#pragma once

#include <functional>
#include <string>

struct Action
{
    std::string name;
    std::function <int ()> func;
};