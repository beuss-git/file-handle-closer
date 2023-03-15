#pragma once

#include <Windows.h>
#include <string>
#include <vector>

struct HandleInfo {
    std::wstring file_name;
    std::wstring process_name;
    ULONG process_id;
    HANDLE file_handle;
};

std::vector<HandleInfo> get_file_handles();

bool close_handle(const HandleInfo& handle);
