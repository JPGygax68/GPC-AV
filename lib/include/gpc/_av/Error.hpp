#pragma once

#include <stdexcept>

#include "config.hpp"

GPC_AV_NAMESPACE_START

class Error: public std::runtime_error {
public:
    Error(int code, const std::string &context): std::runtime_error(make_message(code, context)) {}

protected:
    static auto make_message(int code, const std::string &context) -> std::string;
};

class BadAllocation: public std::runtime_error {
public:
    BadAllocation(const std::string &context) : std::runtime_error(std::string("Allocation error") + context) {}
};

auto get_error_text(int code) -> std::string;

GPC_AV_NAMESPACE_END
