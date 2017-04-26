#pragma once

#include <cassert>
#include <cstdlib>
#include <ostream>
#include <array>
#include <type_traits>

namespace gpc {

    template <typename T> struct is_integer: std::false_type {};
    template <> struct is_integer<short> : std::true_type {};
    template <> struct is_integer<int> : std::true_type {};
    template <> struct is_integer<long> : std::true_type {};
    template <> struct is_integer<long long> : std::true_type {};

    template <typename Int = int>
    class Rational {
    public:

        Rational(Int num_ = static_cast<Int>(0), Int den_ = static_cast<Int>(1)) : num(num_), den(den_) {}

        template <typename IntOther>
        Rational(const Rational<IntOther> &from):
            num(static_cast<Int>(from.numerator())),
            den(static_cast<Int>(from.denominator()))
        {}

        template <typename Duration, typename = Duration::period>
        Rational(const Duration &dur): 
            num(dur.count()), den(Duration::period::den) 
        {}

        auto operator = (const Rational &from)
        {
            num = from.num, den = from.den;
            return *this;
        }

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

        Int numerator() const { return num; }

        Int denominator() const { return den; }

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

        //template <typename Int_> friend std::ostream& operator<< (std::ostream&, const Rational<Int_> &);
        //template <typename Int_> friend Rational<Int_> operator * (Int_ op1, const Rational<Int_> &op2);

        Int num, den;
    };

} // ns gpc

// TODO: replace this with automatic conversion from int to Rational<>

template <typename Int1, typename Int2,
    bool = std::is_integral<Int1>::value,
    bool = std::is_integral<Int2>::value,
    typename Int = std::common_type<Int1, Int2>::type
>
auto operator * (Int1 op1, const gpc::Rational<Int2> &op2) -> gpc::Rational<Int>
{
    return gpc::Rational<Int>(op1) * gpc::Rational<Int>(op2);
}

template <typename Int>
std::ostream& operator << (std::ostream &os, const gpc::Rational<Int> &r)
{
    static_assert(!std::is_enum<Int_>::value);

    os << r.num << "/" << r.den;

    return os;
}