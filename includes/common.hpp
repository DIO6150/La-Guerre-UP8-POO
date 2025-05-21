#pragma once

#include "action.hpp"

#include <vector>

class Pawn;

namespace Common
{
    bool Attack (Pawn *attacker);
    bool Move (Pawn *pawn);
    bool Transform (Pawn *pawn);
    bool CreatePawn (Pawn *pawn);
    
    void PrintBoard ();

    void Play ();

    void Menu ();

    void ClearScreen ();
}