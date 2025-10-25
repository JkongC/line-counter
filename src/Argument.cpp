#include <algorithm>

#include "Argument.h"
#include "utils/Expected.hpp"

InvalidArgument::InvalidArgument(std::string info) : m_Info(std::move(info)) {}

InvalidArgument::InvalidArgument(InvalidArgument &&other) noexcept : m_Info(std::move(other.m_Info)) {}

std::string_view InvalidArgument::info() const
{
    return std::string_view{m_Info.begin(), m_Info.end()};
}

bool ArgItemDefinition::match_full(const std::string_view& item) const
{
    return item == full_name;
}

bool ArgItemDefinition::match_short(const std::string_view &item) const
{
    return item == short_alias;
}

bool ArgItemDefinition::match(const std::string_view& item) const
{
    return match_short(item) || match_full(item);
}

bool ArgItemDefinition::operator==(const std::string_view& item) const
{
    return match(item);
}

template <typename... Args>
constexpr std::unexpected<InvalidArgument> make_invalid_info(std::format_string<Args...> fmt, Args &&...args)
{
    return make_unexpected<InvalidArgument>(fmt, std::forward<Args>(args)...);
}

static std::expected<ArgumentItemCIterType, InvalidArgument> validate_arg_item(std::string_view &item, std::string_view prefix)
{
    size_t item_name_head = item.find_first_not_of("-");
    std::string_view item_name = item.substr(item_name_head, item.size() - item_name_head);
    auto result = std::find_if(argumentItems.begin(), argumentItems.end(), [&](const ArgItemDefinition& def){
        if (prefix == ArgItemDefinition::fullNamePrefix)
            return item_name == def.full_name;
        else
            return item_name == def.short_alias;
    });
    if (result == argumentItems.end())
    {
        return make_invalid_info("Argument item \"{}\" doesn't exist!", item_name);
    }
    return result;
}

std::expected<void, InvalidArgument> ArgumentManager::collect_arguments(int argc, char **argv)
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
        int prefix_flag = 0;
        if (raw.starts_with(ArgItemDefinition::fullNamePrefix))
            prefix_flag = 1;
        else if (raw.starts_with(ArgItemDefinition::aliasPrefix))
            prefix_flag = 2;

        if (prefix_flag == 0)
        {
            if (path_processed)
            {
                if (current_item == argumentItems.end())
                {
                    return make_invalid_info("Argument \"{}\" is not attached to any argument items!", raw);
                }
                m_ArgStorage.emplace(current_item->full_name, raw);
            }
            else
            {
                m_ArgStorage.emplace("work-directory", raw);
                path_processed = true;
            }
        }
        else
        {
            UNWRAP_MOV_EXPECTED_FROM(validate_arg_item(raw, prefix_flag == 1 ? "--" : "-"), current_item);
            path_processed = true;
        }
    }

    if (current_item != argumentItems.end() && m_ArgStorage.find(current_item->full_name) == m_ArgStorage.end())
    {
        m_ArgStorage.emplace(current_item->full_name, "");
    }
    
    if (m_ArgStorage.find("work-directory") == m_ArgStorage.end())
    {
        m_ArgStorage.emplace("work-directory", ".");
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

ArgumentManager::CIterType ArgumentManager::begin() const
{
    return m_ArgStorage.begin();
}

ArgumentManager::CIterType ArgumentManager::end() const
{
    return m_ArgStorage.end();
}
