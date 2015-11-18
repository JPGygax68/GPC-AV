#pragma once

#include <ostream>

#include <gpc/Rational.hpp>

#include "config.hpp"

struct AVRational;

GPC_AV_NAMESPACE_START

class Rational: public ::gpc::Rational<int64_t> {
public:
    typedef gpc::Rational<int64_t> Ancestor;

    using Ancestor::Ancestor;

    Rational() : Ancestor() {}
    Rational(const AVRational &avr);
};

GPC_AV_NAMESPACE_END
