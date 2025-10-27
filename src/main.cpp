#include <print>
#include <filesystem>
#include <istream>

#include "Argument.h"
#include "log/Logger.hpp"

#define PROCESS_ARG_RESULT(result)                                    \
    do                                                                \
    {                                                                 \
        auto _Temp = result;                                          \
        if (!_Temp)                                                   \
        {                                                             \
            log_normal("Argument error: {}", _Temp.error().info()); \
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

    if (ArgumentManager::has_argument("multi-thread"))
        log_normal("Multi-thread mode is on.");
    if (ArgumentManager::has_argument("verbose"))
        log_normal("Verbose mode is on.");
}