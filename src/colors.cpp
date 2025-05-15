#include "colors.hpp"

std::string Dye (std::string str, Color color)
{
    #ifdef ENABLE_COLOR
        std::map<Color, std::string> color_to_escape = {
            {Color::RED, "\x1B[31m"},
            {Color::BLUE, "\x1B[34m"},
            {Color::GOLD, "\x1B[93m"},
            {Color::BLACK, ""},
            {Color::LIGHT_RED, "\x1B[91m"},
            {Color::GREEN, "\x1B[32m"},
            {Color::NORMAL, "\033[0m"},
        };

        return (color_to_escape [color] + str + "\033[0m");
    #else
        return (str);
    #endif

}

Color ColorFromString (std::string str)
{
    std::map<std::string, Color> name_to_color = {
        {"RED", Color::RED},
        {"BLUE", Color::BLUE},
        {"GOLD", Color::GOLD},
        {"BLACK", Color::BLACK},
        {"LRED", Color::LIGHT_RED},
        {"GREEN", Color::GREEN},
        {"", Color::NORMAL},
    };

    if (name_to_color.find (str) != name_to_color.end ())
    {
        return (name_to_color [str]);
    }

    return (Color::NORMAL);
}

std::string StringFromColor (Color color)
{
    std::map<Color, std::string> color_to_key = {
        {Color::RED, "k_red"},
        {Color::BLUE, "k_blue"},
        {Color::GOLD, "k_gold"},
        {Color::BLACK, "k_black"},
        {Color::LIGHT_RED, "k_red"}, // create k_lred in lang
        {Color::GREEN, "k_green"}
    };

    if (color_to_key.find (color) != color_to_key.end ())
    {
        return (color_to_key [color]);
    }

    return ("");
}