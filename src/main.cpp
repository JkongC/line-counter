#include <print>

#include "Argument.h"
#include "log/Logger.hpp"

int main(int argc, char **argv)
{
    auto arg_process_result = ArgumentManager::process_arguments(argc, argv);
    if (!arg_process_result)
    {
        log_normal("Argument invalid: {}", arg_process_result.error().info());
        return 0;
    }

    for (const auto &[item, arg] : ArgumentManager::get_storage())
    {
        std::println("Arg item: {}, Arg content: {}", item, arg);
    }
}