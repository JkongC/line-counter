#if not defined(LC_UTILS_ARRAY_HPP_)

#include <array>

template <typename T, typename... SrcT>
    requires(std::convertible_to<std::decay_t<SrcT>, T> && ...)
inline constexpr auto make_array(SrcT &&...elements)
{
    return std::array<T, sizeof...(elements)>{static_cast<std::decay_t<SrcT>>(elements)...};
}

#define LC_UTILS_ARRAY_HPP_
#endif // LC_UTILS_ARRAY_HPP_