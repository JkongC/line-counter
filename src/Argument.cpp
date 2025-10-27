#include <algorithm>

#include "Argument.h"
#include "utils/Expected.hpp"

InvalidArgument::InvalidArgument(std::string info) : m_Info(std::move(info)) {}

InvalidArgument::InvalidArgument(InvalidArgument &&other) noexcept : m_Info(std::move(other.m_Info)) {}

std::string_view InvalidArgument::info() const
{
    return std::string_view{m_Info.begin(), m_Info.end()};
}

bool ArgCountSpec::too_few_args(int count) const
{
    return count < m_Least;
}

bool ArgCountSpec::too_many_args(int count) const
{
    return m_Least != -1 && count > m_Most;
}

bool ArgCountSpec::proper_arg_count(int count) const
{
    return !too_few_args(count) && !too_many_args(count);
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

template <typename... Args>
constexpr std::unexpected<InvalidArgument> make_too_many_args_info(std::string_view item)
{
    return make_unexpected<InvalidArgument>(TooManyArguments{item}.as_base());
}

using ArgumentItemIterType = decltype(std::begin(argumentItems));
using ArgumentItemCIterType = decltype(std::end(argumentItems));

static std::expected<ArgumentItemCIterType, InvalidArgument> validate_arg_item(std::string_view &item, std::string_view prefix)
{
    size_t item_name_head = item.find_first_not_of("-");
    std::string_view item_name = item.substr(item_name_head, item.size() - item_name_head);
    auto result = std::find_if(std::begin(argumentItems), std::end(argumentItems), [&](const ArgItemDefinition& def){
        if (prefix == ArgItemDefinition::fullNamePrefix)
            return item_name == def.full_name;
        else
            return item_name == def.short_alias;
    });
    if (result == std::end(argumentItems))
    {
        return make_invalid_info("Argument item \"{}\" doesn't exist!", item_name);
    }
    return result;
}

static void insert_to_storage_if_absent(ArgumentManager::StorageType& storage, ArgumentItemCIterType item, std::string_view content = "")
{
    if (item != std::end(argumentItems) && storage.find(item->full_name) == storage.end())
    {
        storage.emplace(item->full_name, content);
    }
}

std::expected<void, InvalidArgument> ArgumentManager::collect_arguments(int argc, char **argv)
{
    std::vector<std::string_view> raw_args;
    raw_args.reserve(argc);
    for (int idx = 1; idx < argc; ++idx)
    {
        raw_args.emplace_back(std::string_view{argv[idx]});
    }

    bool path_processed = false;
    ArgumentItemCIterType current_item = std::end(argumentItems);
    int current_item_args = 0;
    for (auto &raw : raw_args)
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
                if (current_item == std::end(argumentItems))
                {
                    return make_invalid_info("Argument \"{}\" is not attached to any argument items!", raw);
                }
                else if (current_item->arg_count.too_many_args(current_item_args + 1))
                {
                    return make_too_many_args_info(current_item->full_name);
                }

                s_ArgStorage.emplace(current_item->full_name, raw);
                ++current_item_args;
            }
            else
            {
                s_ArgStorage.emplace("work-directory", raw);
                path_processed = true;
            }
        }
        else
        {
            insert_to_storage_if_absent(s_ArgStorage, current_item);
            UNWRAP_MOV_EXPECTED_FROM(validate_arg_item(raw, prefix_flag == 1 ? "--" : "-"), current_item);
            path_processed = true;
            current_item_args = 0;
        }
    }

    insert_to_storage_if_absent(s_ArgStorage, current_item);
    
    if (s_ArgStorage.find("work-directory") == s_ArgStorage.end())
    {
        s_ArgStorage.emplace("work-directory", ".");
    }

    return {};
}

ArgumentManager::Range ArgumentManager::get_argument(std::string_view item)
{
    return Range{ s_ArgStorage.equal_range(item) };
}

bool ArgumentManager::has_argument(std::string_view item)
{
    return s_ArgStorage.find(item) != s_ArgStorage.end();
}

std::vector<std::string_view> ArgumentManager::get_argument_packed(std::string_view item)
{
    auto rg = get_argument(item);
    std::vector<std::string_view> ret;
    for (auto& arg : rg)
    {
        ret.push_back(arg);
    }

    return ret;
}

const ArgumentManager::StorageType &ArgumentManager::get_storage()
{
    return s_ArgStorage;
}

using ArgRange = ArgumentManager::Range;
using ArgIter = ArgRange::Iter;

ArgIter ArgumentManager::Range::begin() const
{
    return ArgIter{ pair_range.first };
}

ArgIter ArgumentManager::Range::end() const
{
    return ArgIter{ pair_range.second };
}

const std::string_view& ArgumentManager::Range::Iter::operator*() const
{
    return pair_iter->second;
}

ArgIter ArgumentManager::Range::Iter::operator++(int)
{
    return ArgIter{ pair_iter++ };
}

ArgIter& ArgumentManager::Range::Iter::operator++()
{
    ++pair_iter;
    return *this;
}

ArgIter ArgumentManager::Range::Iter::operator--(int)
{
    return ArgIter{ pair_iter-- };
}

ArgIter& ArgumentManager::Range::Iter::operator--()
{
    --pair_iter;
    return *this;
}

bool ArgumentManager::Range::Iter::operator==(const ArgumentManager::Range::Iter &other) const
{
    return pair_iter == other.pair_iter;
}

bool ArgumentManager::Range::Iter::operator!=(const ArgumentManager::Range::Iter &other) const
{
    return pair_iter != other.pair_iter;
}
