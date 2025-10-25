#include <print>

#include "Argument.h"
#include "log/Logger.hpp"

#define PROCESS_ARG_RESULT(result)                                    \
    do                                                                \
    {                                                                 \
        auto _Temp = result;                                          \
        if (!_Temp)                                                   \
        {                                                             \
            log_normal("Argument invalid: {}", _Temp.error().info()); \
            return -1;                                                \
        }                                                             \
    } while (0)

    
int process_arguments(int argc, char **argv)
{
    PROCESS_ARG_RESULT(ArgumentManager::collect_arguments(argc, argv));

    return 0;
}

int main(int argc, char **argv)
{
    if (process_arguments(argc, argv) == -1)
        return 0;

    for (const auto &[item, arg] : ArgumentManager::get_storage())
    {
        std::println("Arg item: {}, Arg content: {}", item, arg);
    }
}