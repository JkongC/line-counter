#include <algorithm>

#include "Argument.h"
#include "utils/Expected.hpp"

InvalidArgument::InvalidArgument(std::string info) : m_Info(std::move(info)) {}

InvalidArgument::InvalidArgument(InvalidArgument &&other) noexcept : m_Info(std::move(other.m_Info)) {}

std::string_view InvalidArgument::info() const
{
    return std::string_view{m_Info.begin(), m_Info.end()};
}

template<typename... Args>
constexpr std::unexpected<InvalidArgument> make_invalid_info(std::format_string<Args...> fmt, Args&&... args)
{
    return make_unexpected<InvalidArgument>(fmt, std::forward<Args>(args)...);
}

static std::expected<ArgumentItemCIterType, InvalidArgument> validate_arg_item(std::string_view &item, std::string_view prefix)
{
    size_t item_name_head = item.find_first_not_of(prefix);
    std::string_view item_name = item.substr(item_name_head, item.size() - item_name_head);
    auto result = std::find(argumentItems.begin(), argumentItems.end(), item_name);
    if (result == argumentItems.end())
    {
        return make_invalid_info("Argument item \"{}\" doesn't exist!", item_name);
    }
    return result;
}

std::expected<void, InvalidArgument> ArgumentManager::process_arguments(int argc, char **argv)
{
    std::vector<std::string_view> m_RawArgs;
    m_RawArgs.reserve(argc);
    for (int idx = 1; idx < argc; ++idx)
    {
        m_RawArgs.emplace_back(std::string_view{argv[idx]});
    }

    bool path_processed = false;
    ArgumentItemCIterType current_item = argumentItems.end();
    for (auto &raw : m_RawArgs)
    {
        if (raw.starts_with("--"))
        {
            UNWRAP_MOV_EXPECTED_FROM(validate_arg_item(raw, "--"), current_item);
            m_ArgStorage.emplace(*current_item, "");
            current_item = argumentItems.end();
            path_processed = true;
        }
        else if (raw.starts_with("-"))
        {
            UNWRAP_MOV_EXPECTED_FROM(validate_arg_item(raw, "-"), current_item);
            path_processed = true;
        }
        else if (path_processed)
        {
            if (current_item == argumentItems.end())
            {
                return make_invalid_info("Argument \"{}\" is not attached to any argument items!", raw);
            }
            m_ArgStorage.emplace(*current_item, raw);
        }
        else
        {
            m_ArgStorage.emplace("directory", raw);
            path_processed = true;
        }
    }

    if (m_ArgStorage.find("directory") == m_ArgStorage.end())
    {
        m_ArgStorage.emplace("directory", ".");
    }

    return {};
}

std::optional<ArgumentManager::RangeType> ArgumentManager::get_argument(std::string_view item)
{
    return m_ArgStorage.equal_range(item);
}

const ArgumentManager::StorageType &ArgumentManager::get_storage()
{
    return m_ArgStorage;
}
