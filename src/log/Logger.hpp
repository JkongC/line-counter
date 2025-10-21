#if not defined(LC_LOG_LOGGER_HPP_)

#include <print>

template <typename... Args>
void log_normal(std::format_string<Args...> fmt, Args &&...args)
{
    std::println(fmt, std::forward<Args>(args)...);
}

#define LC_LOG_LOGGER_HPP_
#endif // LC_LOG_LOGGER_HPP_