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
SearchStrategy::Matches RegexSearch::match(std::wstring const& str) {
    try {
        Matches matches;
        std::wsmatch results;

        auto search_start = str.cbegin();
        while (std::regex_search(search_start, str.cend(), results,
                                 m_regex_pattern)) {
            matches.push_back(
                {static_cast<size_t>(results.position(0)),
                 static_cast<size_t>(results.position(0) + results.length(0))});
            search_start = results[0].second;
        }

        return matches;

    } catch (std::regex_error const&) {
        return {};
    }
}
