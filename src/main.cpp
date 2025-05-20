#include "globals.hpp"
#include "board.hpp"
#include "pawn.hpp"
#include "common.hpp"

#include "string_pretty.hpp"

#include <iostream>

#define DEFAULT_LANG "lang/fr_fr.json"

// TODO : replace all occurences of Pawn* with Pawn&& and maybe even more
// TODO : the position of the pawn is not needed inside the Panw class I think, idk, urghhh,
//        like, obviously this is weird but at the same time i don't have to iterate through every pieces in the board and find the right position

int main (__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
    /*
    std::string text = StrColor ("Ceci est une string {C:RED}rouge{} et {X:BLUE}bleue{}. Ca c'est les {C:BLUE, X:RED}deux{}.");
    std::cout << text << std::endl;
    
    int a = 3;
    std::string b = "balbalbal";
    
    std::cout << StrArgs ("#1# #2# #3# #4# ##5## ##1##", a, b, 10) << std::endl;
    
    std::cout << s << std::endl;
    
    std::cout << StrRemoveColor ("{C:YELLOW}Alerte : #1#{}, valeur = #2#") << std::endl;
    */
    
    Globals::LoadLanguage (DEFAULT_LANG);
    
    Globals::GameBoard->AddPawn (new Lord (Color::RED, 0, 0));
    Globals::GameBoard->AddPawn (new Warrior (Color::RED, 1, 1));
    
    Globals::GameBoard->AddPawn (new Lord (Color::BLUE, 19, 19));
    Globals::GameBoard->AddPawn (new Warrior (Color::BLUE, 18, 18));
    Globals::GameBoard->AddPawn (new Castle (Color::BLUE, 19, 18));
    
    //Globals::LoadedLanguage->Print ("k_log_action_attack", Globals::GameBoard->GetPawnAt (0, 0)->GetIcon ().c_str (), 0, 0);

    //std::cout << StrPretty ("Vous avez {C:GOLD}#1#{} d'or.", 10) << std::endl;

    while (Globals::play)
    {
        Common::Play ();
    }
    
    Globals::Cleanup ();
    return (0);
}