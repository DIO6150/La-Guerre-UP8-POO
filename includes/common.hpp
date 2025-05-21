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
    bool LoadGame ();
    bool SaveGame ();
    bool Settings ();
    bool NewGame  ();
    
    void PrintBoard ();
    std::string GetColoredPlayingFaction ();
    int GetConsoleColumns ();
    std::vector <Action> GeneratePlayingActions (bool& continue_turn);
    std::string GetPawnInfo (Pawn *pawn);

    void ClearScreen ();
}