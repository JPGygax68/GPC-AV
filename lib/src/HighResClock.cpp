// This code was contribued on StackOverflow.com by "David" (http://stackoverflow.com/users/369872/david)
// (See http://stackoverflow.com/a/16299576/754534)
// Many thanks!

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

#include <gpc/_av/internal/HighResClock.hpp>

namespace
{
    const long long g_Frequency = []() -> long long 
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return frequency.QuadPart;
    }();
}

namespace gpc {

    HighResClock::time_point HighResClock::now()
    {
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        return time_point(duration(count.QuadPart * static_cast<rep>(period::den) / g_Frequency));
    }

} // ns gpc
