#pragma once
#include <regex>
#include "search_strategy.hpp"

class RegexSearch : public SearchStrategy {
   private:
    RegexSearch() = default;

   public:
    static std::unique_ptr<RegexSearch> create(std::wstring const& pattern);

    Matches match(std::wstring const& str) override;

   private:
    std::wregex m_regex_pattern;
};
