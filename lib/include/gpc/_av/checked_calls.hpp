/* WARNING: DO NOT INCLUDE THIS FILE OUTSIDE OF A PRIVATE TRANSLATION UNIT OF GPC-AV, 
    OTHERWISE THE _call() MACRO MIGHT POLLUTE THE CURRENTLY ACTIVE NAMESPACE!
 */

#include <type_traits>
#include <string>

#include <gpc/_av/config.hpp>
#include <gpc/_av/Error.hpp>

GPC_AV_NAMESPACE_START

inline std::string context_to_string(const char *text, int linenum, const char *file)
{
    using namespace std;

    string s;
    if (text) s += string(text) + " ";

    s += "at line " + to_string(linenum) + " in file \"" + file + "\"";

    return s;
}

inline bool retval_ok(int retval) { return retval >= 0; }

template <typename T>
inline bool retval_ok(T *pointer) { return pointer != nullptr; }

inline void throw_error(int errnum, const std::string &context) { throw Error(errnum, context); }

template <typename T> void throw_error(T *, const std::string &context) { throw BadAllocation(context); }

template <typename Fn, typename... Args>
typename std::result_of<Fn(Args...)>::type
checked_call(const char *text, int line, const char *file, Fn fn, Args&&... args)
{
    using namespace std;

    auto retval = fn(forward<Args>(args)...);
    if (!retval_ok(retval)) throw_error(retval, context_to_string(text, line, file));

    return retval;
}

GPC_AV_NAMESPACE_END

#if defined (_DEBUG)  
#define _av(fn, ...) gpc::av::checked_call(#fn, __LINE__, __FILE__, fn, __VA_ARGS__)
#define AV(fn, ...) gpc::av::checked_call(#fn, __LINE__, __FILE__, fn, __VA_ARGS__)
#else
#define _av(fn, ...) fn(__VA_ARGS__)
#endif  
