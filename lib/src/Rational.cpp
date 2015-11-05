#include <ostream>

extern "C" {
#include "libavutil/rational.h"
}

#include <gpc/_av/Rational.hpp>

GPC_AV_NAMESPACE_START

Rational::Rational(const AVRational & avr)
{
    num = avr.num;
    den = avr.den;
}

auto Rational::reduce() -> Rational&
{
    // TODO: use compile-time 
    static const std::array<int, 10> PRIME_NUMBERS{
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29
    };

    bool reduced;
    do {
        reduced = false;
        for (auto prime : PRIME_NUMBERS)
        {
            // Denominator
            int resd = den / prime, remd = den % prime;
            if (remd != 0) continue;

            // Numerator
            int resn = num / prime, remn = num % prime;
            if (remn != 0) continue;

            // No remainder on either numerator or denominator -> apply
            num = resn, den = resd;
            reduced = true;
        }
    } while (reduced);

    return *this;
}

/*
std::ostream& operator << (std::ostream &os, const Rational &r)
{
    os << r.num << "/" << r.den;

    return os;
}
*/

GPC_AV_NAMESPACE_END
