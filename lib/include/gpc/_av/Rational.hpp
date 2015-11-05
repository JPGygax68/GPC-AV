#pragma once

#include <cstdlib>
#include <ostream>
#include <array>

#include "config.hpp"

GPC_AV_NAMESPACE_START

class Rational {
public:

    Rational() : num(0), den(1) {}

    Rational(int num_, int den_ = 1) : num(num_), den(den_) {}

    auto operator / (const Rational &dsor) -> Rational
    {
        return Rational(num*dsor.den, den*dsor.num).reduce();
    }

    auto operator * (const Rational &muld) -> Rational
    {
        return Rational(num*muld.num, den*muld.den).reduce();
    }

    auto operator + (const Rational &addd) -> Rational
    {
        return Rational(addd.den * num + den * addd.num, den * addd.den).reduce();
    }

    auto operator - (const Rational &addd) -> Rational
    {
        return Rational(addd.den * num - den * addd.num, den * addd.den).reduce();
    }

private:

    friend std::ostream& operator << (std::ostream&, const Rational &);

    auto reduce() -> Rational&
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


    int num, den;
};

inline std::ostream& operator << (std::ostream &os, const Rational &r)
{
    os << r.num << "/" << r.den;

    return os;
}

GPC_AV_NAMESPACE_END
