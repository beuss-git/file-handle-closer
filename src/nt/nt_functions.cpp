#include "nt_functions.hpp"

#include <string_view>

namespace nt {
tNtQuerySystemInformation NtQuerySystemInformation = nullptr;
tNtQueryObject NtQueryObject = nullptr;

template <typename T>
T resolve_nt_function(std::string_view function_name) {
    auto const ntdll_module = GetModuleHandleA("ntdll.dll");
    if (!ntdll_module) {
        return nullptr;
    }

    auto const proc_address =
        GetProcAddress(ntdll_module, function_name.data());
    if (!proc_address) {
        return nullptr;
    }

    // Horrible bypass of "implicit conversion between pointer-to-function" :^)
    union {
        FARPROC proc;
        T func;
    } converter{.proc = proc_address};

    return converter.func;
}

bool resolve_functions() {
    NtQuerySystemInformation = resolve_nt_function<tNtQuerySystemInformation>(
        "NtQuerySystemInformation");
    if (!NtQuerySystemInformation) {
        return false;
    }
    NtQueryObject = resolve_nt_function<tNtQueryObject>("NtQueryObject");
    if (!NtQueryObject) {
        return false;
    }

    return true;
}
}  // namespace nt
