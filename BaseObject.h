#pragma once
#include <memory>


class BaseType {
public:
    virtual ~BaseType() = default;
    virtual std::unique_ptr<BaseType> Clone() const = 0;
};