#pragma once
#include <SubAuth.h>

namespace utils {

inline std::wstring unicode_string_to_wstring(
    UNICODE_STRING const& unicode_string) {
    if (unicode_string.Length == 0) {
        return {};
    }

    wchar_t const* buffer = unicode_string.Buffer;
    const size_t num_utf16_code_units = unicode_string.Length / sizeof(wchar_t);

    return std::wstring{buffer, num_utf16_code_units};
}

inline std::optional<std::string> utf16_to_utf8(
    std::wstring const& utf16_str,
    bool const null_terminate = true) {
    // Get the required buffer size for the converted UTF-8 string
    int const required_buffer_size = WideCharToMultiByte(
        CP_UTF8, 0, utf16_str.c_str(), -1, nullptr, 0, nullptr, nullptr);

    if (required_buffer_size == 0) {
        // WideCharToMultiByte failed
        return {};
    }

    // Allocate the buffer for the converted UTF-8 string
    std::string utf8_str(
        null_terminate ? required_buffer_size : required_buffer_size - 1, '\0');

    // Perform the actual conversion
    int const conversion_result =
        WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), -1, utf8_str.data(),
                            required_buffer_size, nullptr, nullptr);

    if (conversion_result == 0) {
        // WideCharToMultiByte failed
        return {};
    }

    return utf8_str;
}
inline std::optional<std::wstring> utf8_to_utf16(
    std::string const& utf8_str,
    bool const null_terminate = true) {
    // Get the required buffer size for the converted UTF-16 string
    int const required_buffer_size =
        MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);

    if (required_buffer_size == 0) {
        // MultiByteToWideChar failed
        return {};
    }

    // Allocate the buffer for the converted UTF-16 string
    std::wstring utf16_str(
        null_terminate ? required_buffer_size : required_buffer_size - 1, '\0');

    // Perform the actual conversion
    int const conversion_result =
        MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, utf16_str.data(),
                            required_buffer_size);

    if (conversion_result == 0) {
        // MultiByteToWideChar failed
        return {};
    }

    return utf16_str;
}

inline std::wstring to_lower(std::wstring const& s) {
    std::wstring ret = s;
    // This exists because std::tolower returns an int, which \W4 will complain
    // about.
    auto const to_lower_c = [](wchar_t const c) {
        return static_cast<wchar_t>(tolower(c));
    };
    std::ranges::transform(ret, ret.begin(), to_lower_c);
    return ret;
}

inline std::string to_lower(std::string const& s) {
    std::string ret = s;
    // This exists because std::tolower returns an int, which \W4 will complain
    // about.
    auto const to_lower_c = [](char const c) {
        return static_cast<char>(tolower(c));
    };
    std::ranges::transform(ret, ret.begin(), to_lower_c);
    return ret;
}
}  // namespace utils
