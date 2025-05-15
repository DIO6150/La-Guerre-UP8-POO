#pragma once

#include "string_color.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>

class Board;

namespace Globals
{    
    extern std::map<std::string, std::string> Language;
    extern bool play;
    extern std::vector<std::string> ActionLogs;
    
    extern Board *GameBoard;

    void LoadLanguage (std::string language_file);

    template <typename ... Args>
    void Print (std::string string_key, Args ... args)
    {
        std::map<std::string, std::string>::iterator it;
        if ( (it = Language.find (string_key)) == Language.end ())
        {
            std::cout << StrColor ("{RED}Language Key : {GOLD}%s{RED} does not exist.") << std::endl;
            return;
        }

        std::cout << StrColor ((*it).second, args...) << std::endl;
    }

    std::string GetTranslation (std::string string_key);

    void Cleanup ();
}