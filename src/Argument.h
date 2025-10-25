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

struct ArgItemDefinition
{
    static constexpr std::string_view fullNamePrefix = "--";
    static constexpr std::string_view aliasPrefix = "-";
    
    std::string_view full_name{};
    std::string_view short_alias{};

    bool match_full(const std::string_view& item) const;
    bool match_short(const std::string_view& item) const;
    bool match(const std::string_view& item) const;

    bool operator==(const std::string_view& item) const;

    template<std::convertible_to<std::string_view>... Args>
    requires (sizeof...(Args) % 2 == 0)
    static constexpr auto array_from(Args&&... args)
    {
        constexpr size_t count = sizeof...(Args) / 2;
        return [&]<size_t... I>(std::index_sequence<I...>) constexpr {
            const auto tuple = std::make_tuple(args...);
            return std::array<ArgItemDefinition, count>{
                ArgItemDefinition{
                    std::get<I * 2>(tuple),
                    std::get<I * 2 + 1>(tuple)
                }...
            };
        }(std::make_index_sequence<count>());
    }
};

constexpr auto argumentItems = ArgItemDefinition::array_from(
    "directory", "D",
    "thread-count", "Tc",
    "count-all", "Ca");

using ArgumentItemIterType = decltype(argumentItems)::iterator;
using ArgumentItemCIterType = decltype(argumentItems)::const_iterator;

class ArgumentManager
{
public:
    using StorageType = std::multimap<std::string_view, std::string_view>;
    using CIterType = StorageType::const_iterator;
    using RangeType = std::pair<CIterType, CIterType>;

    static std::expected<void, InvalidArgument> collect_arguments(int argc, char **argv);
    static std::optional<RangeType> get_argument(std::string_view item);
    static const StorageType &get_storage();

private:
    ArgumentManager() = delete;
    ArgumentManager(const ArgumentManager &) = delete;
    ArgumentManager(ArgumentManager &&) = delete;
    ArgumentManager &operator=(const ArgumentManager &) = delete;

private:
    static inline StorageType m_ArgStorage;

public:
    CIterType begin() const;
    CIterType end() const;
};

#endif // LC_ARGUMENT_H_