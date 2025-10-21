#if not defined(LC_UTILS_EXPECTED_HPP_)
#define LC_UTILS_EXPECTED_HPP_

#include <expected>

template<typename E, typename... Args>
constexpr std::unexpected<E> make_unexpected(Args&&... args)
{
    return std::unexpected<E>(std::in_place, std::forward<Args>(args)...);
}

#define UNWRAP_EXPECTED(exp, dst) \
    if (exp.has_value())          \
        dst = exp.value();        \
    else                          \
        return std::unexpected(exp.error());

#define UNWRAP_MOV_EXPECTED(exp, dst) \
    if (exp.has_value())              \
        dst = exp.value();            \
    else                              \
        return std::unexpected(std::move(exp).error());

#define UNWRAP_EXPECTED_FROM(rexp, dst)            \
    do                                                  \
    {                                                   \
        auto __UnwrapMacroTempExpected = rexp;     \
        UNWRAP_EXPECTED(__UnwrapMacroTempExpected, dst) \
    } while (0)

#define UNWRAP_MOV_EXPECTED_FROM(rexp, dst)            \
    do                                                      \
    {                                                       \
        auto __UnwrapMacroTempExpected = rexp;         \
        UNWRAP_MOV_EXPECTED(__UnwrapMacroTempExpected, dst) \
    } while (0)

#endif // LC_UTILS_EXPECTED_HPP_