#if not defined(LC_ARGUMENT_H_)
#define LC_ARGUMENT_H_

#include <vector>
#include <string>
#include <string_view>
#include <format>
#include <map>
#include <expected>
#include <optional>

#include "utils/Array.hpp"

inline constexpr auto argumentItems = make_array<std::string_view>(
    "directory",
    "thread-count",
    "count-all");

using ArgumentItemIterType = decltype(argumentItems)::iterator;
using ArgumentItemCIterType = decltype(argumentItems)::const_iterator;

class InvalidArgument
{
public:
    InvalidArgument(std::string info);
    InvalidArgument(InvalidArgument &&other) noexcept;
    InvalidArgument(const InvalidArgument &) = default;

    template <typename... Args>
    InvalidArgument(std::format_string<Args...> fmt, Args &&...args)
        : InvalidArgument(std::format(fmt, std::forward<Args>(args)...)) {}

    std::string_view info() const;

private:
    std::string m_Info;
};

class ArgumentManager
{
public:
    using StorageType = std::multimap<std::string_view, std::string_view>;
    using CIterType = StorageType::const_iterator;
    using RangeType = std::pair<CIterType, CIterType>;

    static std::expected<void, InvalidArgument> process_arguments(int argc, char **argv);
    static std::optional<RangeType> get_argument(std::string_view item);
    static const StorageType &get_storage();

private:
    ArgumentManager();
    ArgumentManager(const ArgumentManager &) = delete;
    ArgumentManager(ArgumentManager &&) = delete;
    ArgumentManager &operator=(const ArgumentManager &) = delete;

private:
    static inline StorageType m_ArgStorage;
};

#endif // LC_ARGUMENT_H_