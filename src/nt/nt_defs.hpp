#pragma once

#include <Windows.h>

#ifndef NT_SUCCESS
#    define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

namespace nt {
static constexpr auto STATUS_INFO_LENGTH_MISMATCH = 0xc0000004;
static constexpr auto SystemHandleInformation = 0x10;
static constexpr auto ObjectNameInformation = 0x1;

using tNtQuerySystemInformation = NTSTATUS(NTAPI*)(ULONG, PVOID, ULONG, PULONG);
using tNtQueryObject = NTSTATUS(NTAPI*)(HANDLE, ULONG, PVOID, ULONG, PULONG);

typedef struct _SYSTEM_HANDLE {
    ULONG ProcessId;
    BYTE ObjectTypeNumber;
    BYTE Flags;
    USHORT Handle;
    PVOID Object;
    ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG HandleCount;
    SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;
}  // namespace nt
