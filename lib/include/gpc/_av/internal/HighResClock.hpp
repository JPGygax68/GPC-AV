#pragma once

// This code was contribued on StackOverflow.com by "David" (http://stackoverflow.com/users/369872/david)
// (See http://stackoverflow.com/a/16299576/754534)
// Many thanks!

#include <chrono>

// TODO: put this class into its own module

namespace gpc {

    struct HighResClock
    {
        typedef long long                               rep;
        typedef std::nano                               period;
        typedef std::chrono::duration<rep, period>      duration;
        typedef std::chrono::time_point<HighResClock>   time_point;
        static const bool is_steady = true;

        static time_point now();
    };

} // ns gpc