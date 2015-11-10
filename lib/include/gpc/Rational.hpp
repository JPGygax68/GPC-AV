#pragma once

#include <cstdlib>
#include <ostream>
#include <array>

namespace gpc {

    template <typename Int = int>
    class Rational {
    public:

        Rational() : num(0), den(1) {}

        Rational(Int num_, Int den_ = static_cast<Int>(1)) : num(num_), den(den_) {}

        template <typename Duration>
        Rational(const Duration &dur) : num(dur.count()), den(Duration::period::den) {}

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

        bool operator < (const Rational &op2)
        {
            return num * op2.den < op2.num * den;
        }

        bool operator > (const Rational &op2)
        {
            return num * op2.den > op2.num * den;
        }

        bool operator == (const Rational &op2)
        {
            return num * op2.den == op2.num * den;
        }

        bool operator != (const Rational &op2)
        {
            return num * op2.den != op2.num * den;
        }

    private:

        auto reduce() -> Rational&
        {
            assert(den != 0);

            // TODO: use compile-time sequence ?
            static const std::array<Int, 10> PRIME_NUMBERS{
                2, 3, 5, 7, 11, 13, 17, 19, 23, 29
            };

            bool reduced;
            do {
                reduced = false;
                for (auto prime : PRIME_NUMBERS)
                {
                    // Denominator
                    Int resd = den / prime, remd = den % prime;
                    if (remd != 0) continue;

                    // Numerator
                    Int resn = num / prime, remn = num % prime;
                    if (remn != 0) continue;

                    // No remainder on either numerator or denominator -> apply
                    num = resn, den = resd;
                    reduced = true;
                }
            } while (reduced);

            return *this;
        }

        //template <typename Int_> friend std::ostream& operator << (std::ostream&, const Rational<Int_> &);
        template <typename Int_> friend Rational<Int> operator * (Int op1, const Rational<Int_> &op2);

        Int num, den;
    };

    template <typename Int>
    std::ostream& operator << (std::ostream &os, const Rational<Int> &r)
    {
        os << r.num << "/" << r.den;

        return os;
    }

    template <typename Int>
    auto operator * (Int op1, const Rational<Int> &op2) -> Rational<Int>
    {
        return Rational<Int>(op1) * op2;
    }

} // ns gpc
