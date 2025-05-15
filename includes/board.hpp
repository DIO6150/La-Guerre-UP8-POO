#pragma once

#include "colors.hpp"

#include <vector>
#include <unordered_map>
#include <memory>

class Pawn;

class Board
{
    std::vector<Pawn *> m_Grid;
    Color m_CurrentFaction;
    std::unordered_map <Color, int> m_Gold;
    
    int m_Size;
    
public:

    Board ();
    ~Board ();

    Pawn *GetPawnAt (int x, int y);
    void MovePawn (Pawn *pawn, int x, int y);
    void SetPawnAt (Pawn* pawn, int x, int y);
    void AddPawn (Pawn* pawn);
    void RemovePawnAt (int x, int y);

    int GetGold (Color faction);
    int GetPlayerGold ();
    void AddPlayerGold (int gold);
    void RemovePlayerGold (int gold);

    int GetSize ();

    std::vector<Pawn *> GetAttackablePawns (Pawn *attacker);
    std::vector<Pawn *> GetAllPawns ();

    Color GetPlayingFaction ();
    void NextPlayer ();
};