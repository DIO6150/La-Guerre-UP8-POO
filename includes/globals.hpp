#pragma once

#include "string_pretty.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>

class Board;

namespace Globals
{
    enum
    {
        MENU,
        GAME,
        QUITTING
    };

    extern int State;

    extern int FirstGold;

    extern std::map<std::string, std::string> Language;
    extern std::vector<std::string> ActionLogs;
    
    extern Board *GameBoard;

    void LoadLanguage (std::string language_file);

    template <typename... Args>
    void Print (std::string string_key, Args&&... args)
    {
        std::map<std::string, std::string>::iterator it;
        if ( (it = Language.find (string_key)) == Language.end ())
        {
            std::cout << StrPretty ("{C:RED}Language Key: {C:YELLOW}#1#{C:RED} does not exist.", string_key) << std::endl;
            return;
        }

        std::cout << StrPretty ((*it).second, std::forward<Args>(args)...) << std::endl;
    }

    template <typename... Args>
    void PrintPretty (std::string text, Args&&... args)
    {
        std::cout << StrPretty (text, std::forward<Args>(args)...) << std::endl;
    }

    std::string GetTranslation (std::string string_key, bool remove_color = false);
    
    template <typename... Args>
    std::string GetPrettyfiedTranslation (std::string string_key, Args&&... args)
    {
        return (StrPretty (GetTranslation (string_key), std::forward<Args> (args)...));
    }
    
    void Cleanup ();
}