#include "globals.hpp"

#include "board.hpp"

#include "json.hpp"

#include <fstream>
#include <iostream>

using json = nlohmann::json;

int Globals::State = Globals::MENU;

int Globals::FirstGold = 0;

std::map<std::string, std::string> Globals::Language = {};
std::vector<std::string> Globals::ActionLogs = {};

Board *Globals::GameBoard = new Board {};

void Globals::LoadLanguage (std::string language_file)
{
    Language.clear ();

    std::ifstream file;

    try
    {
        file.open (language_file);
    }
    catch (std::ifstream::failure& ex)
    {
        Globals::PrintPretty ("{C:RED}Couldn't open language file: {C:GOLD}#1#{C:RED}.", language_file);
        return;
    }


    std::string line;
    std::string text_file;

    while (std::getline (file, line)) text_file += line;

    json data = {};
    try
    {
        data = json::parse (text_file);
    }
    catch (json::parse_error& ex)
    {
        Globals::PrintPretty ("{C:RED}Couldn't parse language: {C:GOLD}#1#{C:RED}.", text_file);
        return;
    }

    for (auto& e : data.items ()) Language [e.key ()] = e.value ();

    file.close ();
}

std::string Globals::GetTranslation (std::string string_key, bool remove_color)
{
    std::map<std::string, std::string>::iterator it;
    if ((it = Language.find (string_key)) == Language.end ()) return (StrPretty ("{C:RED}Language Key : {C:YELLOW}#1#{C:RED} does not exist.", string_key));
    if (remove_color) return (StrRemoveColor ((*it).second));
    return ((*it).second);
}

void Globals::Cleanup ()
{
    delete GameBoard;
}