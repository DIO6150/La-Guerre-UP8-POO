#include "globals.hpp"
#include "board.hpp"
#include "pawn.hpp"
#include "common.hpp"
#include "string_pretty.hpp"
#include "utils.hpp"
#include "scenes.hpp"

#include <iostream>

#define DEFAULT_LANG "lang/fr_fr.json"



int main (__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
    Globals::LoadLanguage (DEFAULT_LANG);

    while (true)
    {
        switch (Globals::State)
        {
        case Globals::MENU:
            Menu ();
            break;

        case Globals::GAME:
            Play ();
        
        default:
            break;
        }

        if (Globals::State == Globals::QUITTING) break;
    }
    
    Common::ClearScreen ();
    Globals::Cleanup ();
    return (0);
}