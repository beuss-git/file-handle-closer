#pragma once
#include "search_strategy.hpp"

class PatternSearch : public SearchStrategy {
   private:
    PatternSearch() = default;

   public:
    static std::unique_ptr<PatternSearch> create(std::wstring const& pattern);
    Matches match(std::wstring const& str) override;

   private:
    std::wstring m_pattern;
};
