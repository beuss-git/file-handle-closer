#include "pattern_search.hpp"
#include <iostream>

std::unique_ptr<PatternSearch> PatternSearch::create(
    std::wstring const& pattern) {
    auto instance = std::unique_ptr<PatternSearch>(new PatternSearch());
    if (pattern.empty()) {
        return nullptr;
    }

    instance->m_pattern = pattern;
    // Add implicit * at the beginning of the pattern
    if (instance->m_pattern[0] != L'*') {
        instance->m_pattern = L"*" + instance->m_pattern;
    }
    // Add implicit * at the end of the pattern
    if (instance->m_pattern[instance->m_pattern.size() - 1] != L'*') {
        instance->m_pattern += L"*";
    }
    return instance;
}
SearchStrategy::Matches PatternSearch::match(std::wstring const& str) {
    size_t str_index = 0, pattern_index = 0;
    size_t star_index = std::wstring::npos, str_tmp_index = 0;
    size_t match_start = 0;

    while (str_index <= str.size()) {
        if (pattern_index < m_pattern.size() &&
            (str[str_index] == m_pattern[pattern_index] ||
             m_pattern[pattern_index] == L'?')) {
            if (pattern_index == 1) {
                match_start = str_index;
            }
            str_index++;
            pattern_index++;
        } else if (pattern_index < m_pattern.size() &&
                   m_pattern[pattern_index] == L'*') {
            star_index = pattern_index++;
            str_tmp_index = str_index;
        } else if (star_index != std::wstring::npos) {
            pattern_index = star_index + 1;
            str_index = ++str_tmp_index;
        } else {
            break;
        }

        if (pattern_index == m_pattern.size()) {
            return Matches{{match_start, str_index}};
        }
    }
    return {};
}
