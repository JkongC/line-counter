#include <algorithm>

#include "Argument.h"
#include "log/Logger.hpp"

static ArgumentItemCIterType validate_arg_item(std::string_view &item, std::string_view prefix)
{
    size_t item_name_head = item.find_first_not_of(prefix);
    std::string_view item_name = item.substr(item_name_head, item.size() - item_name_head);
    auto result = std::find(argumentItems.begin(), argumentItems.end(), item_name);
    if (result == argumentItems.end())
    {
        log_normal("Argument item \"{}\" doesn't exist!", item_name);
        throw InvalidArgument{};
    }
    return result;
}

void ArgumentManager::process_arguments(int argc, char **argv)
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
            current_item = validate_arg_item(raw, "--");
            m_ArgStorage.emplace(*current_item, "");
            current_item = argumentItems.end();
            path_processed = true;
        }
        else if (raw.starts_with("-"))
        {
            current_item = validate_arg_item(raw, "-");
            path_processed = true;
        }
        else
        {
            if (path_processed)
            {
                if (current_item == argumentItems.end())
                {
                    log_normal("Argument \"{}\" is not attached to any argument items!", raw);
                    throw InvalidArgument{};
                }
                m_ArgStorage.emplace(*current_item, raw);
            }
            else
            {
                m_ArgStorage.emplace("directory", raw);
                path_processed = true;
            }
        }
    }
}

ArgumentManager::RangeType ArgumentManager::get_argument(std::string_view item)
{
    return m_ArgStorage.equal_range(item);
}

const ArgumentManager::StorageType &ArgumentManager::get_storage()
{
    return m_ArgStorage;
}
