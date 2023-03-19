#pragma once
#include <memory>
#include <string>

class SearchStrategy {
   public:
    virtual ~SearchStrategy() = default;
    virtual bool match(std::wstring const& str) = 0;
};
