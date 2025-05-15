#include "string_color.hpp"

#include <vector>
#include <utility>

static std::string _StrColor (std::string str)
{
    std::vector<std::pair<std::string, Color>> colored_token;

    std::string str_token   = "";
    std::string color_token = "";

    bool color_mode = false;
    unsigned int closing_bracket = -1;

    
    for (int i = 0; i < (int) str.length (); ++i)
    {
        if (str [i] == opening_brackets)
        {
            if ((closing_bracket = str.find (closing_brackets, i + 1)) == std::string::npos) return (Dye ("Error when parsing string.", Color::RED));
            
            if (i != 0) colored_token.push_back ({str_token, ColorFromString (color_token)});
            
            str_token = "";
            color_token = "";
            
            color_mode = true;

            continue;
        }

        if (i == (int) closing_bracket)
        {
            closing_bracket = -1;
            color_mode = false;
            continue;
        }

        if (color_mode) color_token += str [i];
        else str_token += str [i];
    }

    if (str_token != "") colored_token.push_back ({str_token, ColorFromString (color_token)});

    std::string colored_str = "";

    for (auto& c : colored_token)
    {
        colored_str += Dye (c.first, c.second);
    }

    return (colored_str);
}

std::string StrColor (std::string str, ...)
{
    va_list args;
    va_start (args, str);

    std::size_t pos = -1;

    while ((pos = str.find (replacer, pos + 1)) != std::string::npos)
    {
        char format_char = str [pos + 1];
        str.erase (pos, 2);

        switch (format_char)
        {
        case r_char:
            str.insert (pos, 1, (char) va_arg (args, int));
            break;

        case r_double:
            str.insert (pos, std::to_string ((double) va_arg (args, double)));
            break;

        case r_int:
            str.insert (pos, std::to_string ((int) va_arg (args, int)));
            break;

        case r_string:
            str.insert (pos, (char *) va_arg (args, char *));
            break;
        
        default:
            return (Dye ("Unknown format character : " + str [pos + 1], Color::RED));
            break;
        }
    }

    va_end (args);

    return (_StrColor (str));
}