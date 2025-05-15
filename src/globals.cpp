#include "globals.hpp"

#include "board.hpp"

#include "json.hpp"

#include <fstream>
#include <iostream>

using json = nlohmann::json;

std::map<std::string, std::string> Globals::Language = {};
bool Globals::play = true;
std::vector<std::string> Globals::ActionLogs = {};

Board *Globals::GameBoard = new Board {};

void Globals::LoadLanguage (std::string language_file)
{
    // CurrentLanguage = language_file;
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

std::string Globals::GetTranslation (std::string string_key)
{
    std::map<std::string, std::string>::iterator it;
    if ( (it = Language.find (string_key)) == Language.end ())
    {
        return (StrColor ("{RED}Language Key : {GOLD}%s{RED} does not exist.", string_key.c_str ()));
    }
    
    // std::cout << (*it).first << " : " << (*it).second << std::endl;
    
    return ((*it).second);
}

void Globals::Cleanup ()
{
    delete GameBoard;
}