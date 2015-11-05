#include <ostream>

extern "C" {
#include "libavutil/rational.h"
}

#include <gpc/_av/Rational.hpp>

GPC_AV_NAMESPACE_START

Rational::Rational(const AVRational & avr): Ancestor(avr.num, avr.den) {}

GPC_AV_NAMESPACE_END
