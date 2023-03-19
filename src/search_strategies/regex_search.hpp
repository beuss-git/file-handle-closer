#pragma once
#include <regex>
#include "search_strategy.hpp"

class RegexSearch : public SearchStrategy {
   private:
    RegexSearch() = default;

   public:
    static std::unique_ptr<RegexSearch> create(const std::wstring& pattern);

    bool match(const std::wstring& str) override;

   private:
    std::wregex m_regex_pattern;
};
