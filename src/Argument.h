#if not defined(LC_ARGUMENT_H_)
#define LC_ARGUMENT_H_

#include <vector>
#include <string_view>
#include <array>
#include <map>
#include <exception>

#include "utils/Array.hpp"

inline constexpr auto argumentItems = make_array<std::string_view>(
    "directory");

using ArgumentItemIterType = decltype(argumentItems)::iterator;
using ArgumentItemCIterType = decltype(argumentItems)::const_iterator;

class InvalidArgument : std::exception
{
};

class ArgumentManager
{
public:
    using StorageType = std::multimap<std::string_view, std::string_view>;
    using CIterType = StorageType::const_iterator;
    using RangeType = std::pair<CIterType, CIterType>;

    static void process_arguments(int argc, char **argv);
    static RangeType get_argument(std::string_view item);
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