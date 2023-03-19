#include "pattern_search.hpp"

std::unique_ptr<PatternSearch> PatternSearch::create(
    std::wstring const& pattern) {
    auto instance = std::unique_ptr<PatternSearch>(new PatternSearch());
    instance->m_pattern = pattern;
    return instance;
}
bool PatternSearch::match(std::wstring const& str) {
    size_t str_index = 0, pattern_index = 0;
    size_t star_index = std::wstring::npos, str_tmp_index = 0;

    while (str_index < str.size()) {
        if (pattern_index < m_pattern.size() &&
            (str[str_index] == m_pattern[pattern_index] ||
             m_pattern[pattern_index] == L'?')) {
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
            return false;
        }
    }

    while (pattern_index < m_pattern.size() &&
           m_pattern[pattern_index] == L'*') {
        pattern_index++;
    }

    return pattern_index == m_pattern.size();
}
