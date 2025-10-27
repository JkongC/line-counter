#if not defined(LC_ARGUMENT_H_)
#define LC_ARGUMENT_H_

#include <vector>
#include <string>
#include <string_view>
#include <format>
#include <map>
#include <expected>
#include <span>

#include "utils/Array.hpp"

struct InvalidArgument
{
public:
    InvalidArgument(std::string info);
    InvalidArgument(InvalidArgument&& other) noexcept;
    InvalidArgument(const InvalidArgument &) = default;

    template <typename... Args>
    InvalidArgument(std::format_string<Args...> fmt, Args&&... args)
        : InvalidArgument(std::format(fmt, std::forward<Args>(args)...)) {}

    template<typename Self>
    decltype(auto) as_base(this Self&& self)
    {
        if constexpr (std::is_lvalue_reference_v<Self>)
            return static_cast<InvalidArgument&>(self);
        else
            return static_cast<InvalidArgument&&>(self);
    }

    std::string_view info() const;

private:
    std::string m_Info;
};

struct TooManyArguments : public InvalidArgument
{    
    template<typename Arg, typename... Args>
    static void _mals_impl(std::string& s, Arg&& arg, Args&&... args)
    {
        if constexpr (std::is_same_v<Arg, std::string_view>)
        {
            s += arg;
        }
        else if constexpr (std::convertible_to<Arg, std::string_view>)
        {
            s += std::string_view{ arg };
        }
        else
        {
            s += std::format("{}", arg);
        }

        _mals_impl(s, args...);
    }

    template<typename Arg, typename... Args>
    static std::string make_argument_list_string(Args&&... args)
    {
        std::string ret;
        _mals_impl(ret, args...);
        return ret;
    }
    
    TooManyArguments(std::string_view item)
        : InvalidArgument("Too many arguments for item \"{}\".", item) {}

    template<typename... Args>
    TooManyArguments(std::string_view item, size_t count, Args&&... args)
        : InvalidArgument("Too many arguments({}) for item \"{}\":\n{}", count, item, make_argument_list_string(args...)) {}
};

struct ArgCountSpec
{
public:
    static constexpr ArgCountSpec unlimited()
    {
        return ArgCountSpec(-1, -1);
    }

    static constexpr ArgCountSpec no_arg()
    {
        return ArgCountSpec(0, 0);
    }

    static constexpr ArgCountSpec between(int least, int most)
    {
        return ArgCountSpec(least, most);
    }

    static constexpr ArgCountSpec specific(int count)
    {
        return ArgCountSpec(count, count);
    }

public:
    bool too_many_args(int count) const;
    bool too_few_args(int count) const;
    bool proper_arg_count(int count) const;

private:
    constexpr ArgCountSpec(int least, int most) : m_Least(least), m_Most(most) {}

private:
    int m_Least;
    int m_Most;
};

struct ArgItemDefinition
{
    static constexpr std::string_view fullNamePrefix = "--";
    static constexpr std::string_view aliasPrefix = "-";

    std::string_view full_name{};
    std::string_view short_alias{};
    ArgCountSpec arg_count = ArgCountSpec::unlimited();

    bool match_full(const std::string_view &item) const;
    bool match_short(const std::string_view &item) const;
    bool match(const std::string_view &item) const;

    bool operator==(const std::string_view &item) const;
};

class ArgumentManager
{
public:
    using StorageType = std::multimap<std::string_view, std::string_view>;
    using StorageCIterType = StorageType::const_iterator;
    using StorageRangeType = std::pair<StorageCIterType, StorageCIterType>;

    struct Range
    {
        StorageRangeType pair_range;
        
        struct Iter
        {
            StorageCIterType pair_iter;

            const std::string_view& operator*() const;

            Iter operator++(int);
            Iter& operator++();

            Iter operator--(int);
            Iter& operator--();

            bool operator==(const Iter& other) const;
            bool operator!=(const Iter& other) const;
        };

        Iter begin() const;
        Iter end() const;
    };

    static std::expected<void, InvalidArgument> collect_arguments(int argc, char **argv);
    static Range get_argument(std::string_view item);
    static bool has_argument(std::string_view item);
    static std::vector<std::string_view> get_argument_packed(std::string_view item);
    static const StorageType &get_storage();

private:
    ArgumentManager() = delete;
    ArgumentManager(const ArgumentManager &) = delete;
    ArgumentManager(ArgumentManager &&) = delete;
    ArgumentManager &operator=(const ArgumentManager &) = delete;

private:
    static inline StorageType s_ArgStorage;
};

// Define all valid argument items here.
constexpr ArgItemDefinition argumentItems[]
{
    {"directory", "D"},
    {"multi-thread", "MT", ArgCountSpec::no_arg()},
    {"verbose", "V", ArgCountSpec::no_arg()}
};

#endif // LC_ARGUMENT_H_