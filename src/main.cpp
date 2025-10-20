#include <print>

#include "Argument.h"

int main(int argc, char **argv)
{
    ArgumentManager::process_arguments(argc, argv);
    for (const auto& [item, arg] : ArgumentManager::get_storage())
    {
        std::println("Arg item: {}, Arg content: {}", item, arg);
    }
}