#include "debug_privileges.hpp"

#include <Windows.h>

#include "handle_guard.hpp"

namespace utils {
bool set_privilege(HANDLE hToken, LPCTSTR lpPrivilege, BOOL bEnablePrivilege) {
    // Check the parameters passed to the function
    if (!hToken || !lpPrivilege) {
        return false;
    }

    LUID luid{};
    if (!LookupPrivilegeValue(nullptr, lpPrivilege, &luid)) {
        return false;
    }

    TOKEN_PRIVILEGES tkp{};
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luid;
    tkp.Privileges[0].Attributes = 0;

    DWORD cb_previous = sizeof(TOKEN_PRIVILEGES);

    TOKEN_PRIVILEGES tkp_previous{};
    AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES),
                          &tkp_previous, &cb_previous);
    if (GetLastError() != ERROR_SUCCESS) {
        return false;
    }

    tkp_previous.PrivilegeCount = 1;
    tkp_previous.Privileges[0].Luid = luid;

    if (bEnablePrivilege) {
        tkp_previous.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
    } else {
        tkp_previous.Privileges[0].Attributes ^=
            (SE_PRIVILEGE_ENABLED & tkp_previous.Privileges[0].Attributes);
    }

    AdjustTokenPrivileges(hToken, FALSE, &tkp_previous, cb_previous, nullptr,
                          nullptr);
    if (GetLastError() != ERROR_SUCCESS) {
        return false;
    }

    return true;
}

bool set_debug_privilege(bool enable) {
    auto token = HandleGuard::create();

    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                          &token.get())) {
        return false;
    }

    // Enable/Disable Debug Privilege
    if (!set_privilege(token.get(), SE_DEBUG_NAME, enable)) {
        return false;
    }

    return true;
}
}  // namespace utils
