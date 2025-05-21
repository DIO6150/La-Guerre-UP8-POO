#include "globals.hpp"

#include "board.hpp"

#include "json.hpp"

#include <fstream>
#include <iostream>

using json = nlohmann::json;

int Globals::State = Globals::MENU;
std::map<std::string, std::string> Globals::Language = {};
std::vector<std::string> Globals::ActionLogs = {};

Board *Globals::GameBoard = new Board {};

void Globals::LoadLanguage (std::string language_file)
{
    // TODO : make it prettyier by copying file content to json without getline
    Language.clear ();

    std::ifstream file {language_file};
    std::string line;
    std::string text_file;
    while (std::getline (file, line))
    {
        text_file += line;
    }

    json data = json::parse (text_file);

    for (auto& e : data.items ())
    {
        Language [e.key ()] = e.value ();
    }

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