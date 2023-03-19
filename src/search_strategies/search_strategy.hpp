#pragma once
#include <memory>
#include <span>
#include <string>
#include <vector>

class SearchStrategy {
   public:
    virtual ~SearchStrategy() = default;

    struct Match {
        std::size_t start;
        std::size_t end;
    };

    using Matches = std::vector<Match>;

    virtual Matches match(std::wstring const& str) = 0;
};
