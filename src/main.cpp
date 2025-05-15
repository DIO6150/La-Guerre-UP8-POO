#include "globals.hpp"
#include "board.hpp"
#include "pawn.hpp"
#include "string_color.hpp"
#include "common.hpp"

#include <iostream>

#define DEFAULT_LANG "lang/fr_fr.json"

// TODO : replace all occurences of Pawn* with Pawn&& and maybe even more
// TODO : the position of the pawn is not needed inside the Panw class I think, idk, urghhh,
//        like, obviously this is weird but at the same time i don't have to iterate through every pieces in the board and find the right position

int main (__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
    //std::cout << StrColor ("Vous avez {GOLD}....{} d'Or.") << std::endl;
    
    // Pawn::Test (Lord (Color::RED, 0, 0));

    Globals::LoadLanguage (DEFAULT_LANG);

    Globals::GameBoard->AddPawn (new Lord (Color::RED, 0, 0));
    Globals::GameBoard->AddPawn (new Warrior (Color::RED, 1, 1));

    Globals::GameBoard->AddPawn (new Lord (Color::BLUE, 19, 19));
    Globals::GameBoard->AddPawn (new Warrior (Color::BLUE, 18, 18));
    Globals::GameBoard->AddPawn (new Castle (Color::BLUE, 19, 18));

    //Globals::LoadedLanguage->Print ("k_action_log_attack", Globals::GameBoard->GetPawnAt (0, 0)->GetIcon ().c_str (), 0, 0);

    while (Globals::play)
    {
        Common::Play ();
    }
   
    Globals::Cleanup ();
    return (0);
}