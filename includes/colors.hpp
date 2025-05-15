#pragma once

#include <string>
#include <map>

#define ENABLE_COLOR

enum class Color
{
    RED, BLUE, GOLD, BLACK, LIGHT_RED, GREEN,
    NORMAL
};

std::string Dye (std::string str, Color color);
Color ColorFromString (std::string str);
std::string StringFromColor (Color color);