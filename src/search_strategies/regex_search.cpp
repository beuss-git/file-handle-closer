#include "regex_search.hpp"
#include <iostream>
#include <memory>
#include <regex>

std::unique_ptr<RegexSearch> RegexSearch::create(std::wstring const& pattern) {
    auto instance = std::unique_ptr<RegexSearch>(new RegexSearch());
    try {
        instance->m_regex_pattern = std::wregex(pattern);
        return instance;
    } catch (std::invalid_argument const&) {
        std::cerr << "Invalid regex pattern\n";
        return nullptr;
    }
}
bool RegexSearch::match(std::wstring const& str) {
    try {
        return std::regex_search(str, m_regex_pattern);
    } catch (std::regex_error const&) {
        return false;
    }
}
