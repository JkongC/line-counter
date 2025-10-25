#if not defined(LC_UTILS_ARRAY_HPP_)

#include <array>

template<typename T, typename... Args>
consteval void _ConstructArray(Args&&... args)
{
    T arr[]{args...};
}

template <typename T, typename... Args>
concept CanConstructArray = requires {
    _ConstructArray(std::declval<Args>()...);
};

template <typename T, std::convertible_to<T>... Ele>
constexpr auto make_array(Ele &&...elements)
{
    if constexpr (CanConstructArray<T, Ele...>)
    {
        return std::array<T, sizeof...(Ele)>{elements...};
    }
    else
    {
        return std::array<T, sizeof...(Ele)>{static_cast<std::decay_t<Ele>>(elements)...};
    }
}

#define LC_UTILS_ARRAY_HPP_
#endif // LC_UTILS_ARRAY_HPP_