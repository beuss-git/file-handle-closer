#include "regex_search.hpp"
#include <iostream>
#include <memory>
#include <regex>

std::unique_ptr<RegexSearch> RegexSearch::create(const std::wstring& pattern) {
    auto instance = std::unique_ptr<RegexSearch>(new RegexSearch());
    try {
        instance->m_regex_pattern = std::wregex(pattern);
        return instance;
    } catch (const std::invalid_argument&) {
        std::cerr << "Invalid regex pattern\n";
        return nullptr;
    }
}
bool RegexSearch::match(const std::wstring& str) {
    try {
        return std::regex_search(str, m_regex_pattern);
    } catch (const std::regex_error&) {
        return false;
    }
}
