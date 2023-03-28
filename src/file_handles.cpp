#include "file_handles.hpp"

#include <Psapi.h>
#include <SubAuth.h>
#include <Windows.h>
#include <algorithm>
#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <vector>

#include "nt/nt_defs.hpp"
#include "nt/nt_functions.hpp"
#include "utils/handle_guard.hpp"
#include "utils/string_utils.hpp"

std::optional<std::wstring> get_object_name(HANDLE dupe_handle) {
    constexpr size_t buffer_size = 0x1000;

    // Allocate a buffer to store the object name information
    std::vector<unsigned char> object_name_buf(buffer_size);

    ULONG return_length;
    NTSTATUS status =
        nt::NtQueryObject(dupe_handle, nt::ObjectNameInformation,
                          object_name_buf.data(), buffer_size, &return_length);

    // If the buffer size is insufficient, resize the buffer and try again
    if (status == nt::STATUS_INFO_LENGTH_MISMATCH) {
        object_name_buf.resize(return_length);
        status = nt::NtQueryObject(dupe_handle, nt::ObjectNameInformation,
                                   object_name_buf.data(), return_length,
                                   &return_length);
    }

    if (!NT_SUCCESS(status)) {
        return {};
    }

    auto const unicode_string =
        reinterpret_cast<PUNICODE_STRING>(object_name_buf.data());

    if (unicode_string->Length == 0) {
        return {};
    }

    return utils::unicode_string_to_wstring(*unicode_string);
}

std::wstring canonicalize_nt_path_name(std::wstring const& nt_path_name) {
    std::wstring canonicalized = nt_path_name;

    std::wstring lower_nt_path_name = utils::to_lower(nt_path_name);

    if (const DWORD drive_mask = GetLogicalDrives()) {
        for (int i = 0; i < 26; ++i) {
            if (!(drive_mask & (1 << i)))
                continue;
            const std::wstring drive =
                std::format(L"{}:", static_cast<wchar_t>('A' + i));
            const std::wstring nt_drive_name(MAX_PATH + 1, '\0');

            if (QueryDosDeviceW(drive.c_str(),
                                const_cast<wchar_t*>(nt_drive_name.c_str()),
                                MAX_PATH) == 0)
                continue;

            auto const lower_nt_drive_name = utils::to_lower(nt_drive_name);
            // Remove the trailing null characters to do the comparison
            auto const view =
                std::wstring_view{lower_nt_drive_name.data(),
                                  std::wcslen(lower_nt_drive_name.c_str())};

            // Check that it starts with the drive name
            if (lower_nt_path_name.find(view) == 0) {
                canonicalized = std::format(L"{}{}", drive,
                                            nt_path_name.substr(view.length()));
                break;
            }
        }
    }

    return canonicalized;
}

std::optional<std::wstring> get_process_name(DWORD process_id) {
    wchar_t sz_process_name[MAX_PATH] = {0};
    auto const proc_handle = utils::HandleGuard(OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id));
    if (!proc_handle) {
        return {};
    }
    if (GetProcessImageFileNameW(proc_handle.get(), sz_process_name,
                                 MAX_PATH) == 0) {
        return {};
    }
    return {sz_process_name};
}

std::vector<HandleInfo> get_file_handles() {
    NTSTATUS status;
    const ULONG current_process_id = GetCurrentProcessId();

    std::vector<BYTE> system_handle_buf;
    ULONG system_info_length = sizeof(nt::SYSTEM_HANDLE_INFORMATION) +
                               (sizeof(nt::SYSTEM_HANDLE) * 100);

    system_handle_buf.resize(system_info_length);

    while ((status = nt::NtQuerySystemInformation(
                nt::SystemHandleInformation,
                reinterpret_cast<nt::PSYSTEM_HANDLE_INFORMATION>(
                    system_handle_buf.data()),
                system_info_length, nullptr)) ==
           static_cast<NTSTATUS>(nt::STATUS_INFO_LENGTH_MISMATCH)) {
        system_info_length *= 2;
        system_handle_buf.resize(system_info_length);
    }

    if (!NT_SUCCESS(status)) {
        return {};
    }

    auto const handle_info = reinterpret_cast<nt::PSYSTEM_HANDLE_INFORMATION>(
        system_handle_buf.data());

    std::vector<HandleInfo> handles_list{};
    for (unsigned int i = 0; i < handle_info->HandleCount; i++) {
        const nt::SYSTEM_HANDLE system_handle = handle_info->Handles[i];

        ULONG process_id = system_handle.ProcessId;

        if (current_process_id == process_id) {
            continue;
        }

        if (system_handle.Handle == 0) {
            continue;
        }

        utils::HandleGuard process_guard(
            OpenProcess(PROCESS_DUP_HANDLE, TRUE, process_id));
        if (!process_guard) {
            continue;
        }

        auto dup_handle = utils::HandleGuard::create();
        status = DuplicateHandle(process_guard.get(),
                                 reinterpret_cast<HANDLE>(system_handle.Handle),
                                 GetCurrentProcess(), &dup_handle.get(), 0,
                                 FALSE, DUPLICATE_SAME_ACCESS);

        if (!NT_SUCCESS(status)) {
            continue;
        }

        // OPTIMIZATION: Bail out if the handle is not a file handle
        //               This also avoids the need to query the object type
        //               And finally it avoids NtQueryObject() hanging on some
        //               handles due to deadlocks and mutexes or other
        //               synchronization objects
        if (GetFileType(dup_handle.get()) != FILE_TYPE_DISK) {
            continue;
        }

        auto const file_name = get_object_name(dup_handle.get());
        if (!file_name) {
            continue;
        }

        auto const process_path = get_process_name(process_id);
        if (!process_path) {
            continue;
        }
        auto const process_name =
            std::filesystem::path(process_path.value()).filename().wstring();

        handles_list.emplace_back(
            HandleInfo{canonicalize_nt_path_name(file_name.value()),
                       canonicalize_nt_path_name(process_name), process_id,
                       reinterpret_cast<HANDLE>(system_handle.Handle)});
    }

    return handles_list;
}

bool close_handle(HandleInfo const& handle) {
    const utils::HandleGuard process_handle(
        OpenProcess(PROCESS_DUP_HANDLE, TRUE, handle.process_id));

    if (!process_handle) {
        return false;
    }

    auto dup_handle = utils::HandleGuard::create();
    NTSTATUS status = DuplicateHandle(process_handle.get(), handle.file_handle,
                                      GetCurrentProcess(), &dup_handle.get(), 0,
                                      FALSE, DUPLICATE_CLOSE_SOURCE);

    if (!NT_SUCCESS(status)) {
        std::cerr << std::format("DuplicateHandle failed with status: 0x{:08X}",
                                 status)
                  << "\n";
        return false;
    }

    return true;
}

bool kill_process(HandleInfo const& handle) {
    const utils::HandleGuard process_handle(
        OpenProcess(PROCESS_TERMINATE, TRUE, handle.process_id));

    if (!process_handle) {
        return false;
    }

    if (!TerminateProcess(process_handle.get(), 0)) {
        std::cerr << std::format("TerminateProcess failed with error: {}",
                                 GetLastError())
                  << "\n";
        return false;
    }

    return true;
}