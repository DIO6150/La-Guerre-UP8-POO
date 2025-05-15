#include "board.hpp"
#include "pawn.hpp"
#include "colors.hpp"

#include <iostream>

#define SIZE 20

Board::Board () : 
m_Grid {SIZE * SIZE, nullptr},
m_CurrentFaction { Color::RED },
m_Gold { 
        {Color::RED, 15},
        {Color::BLUE, 15}
    },
m_Size (SIZE)
{

}

Board::~Board ()
{
    
}

Pawn *Board::GetPawnAt (int x, int y)
{
    return (m_Grid [x + SIZE * y]);
}

void Board::MovePawn (Pawn *pawn, int x, int y)
{
    SetPawnAt (nullptr, pawn->m_PositionX, pawn->m_PositionY);
    SetPawnAt (pawn, x, y);
}

void Board::SetPawnAt (Pawn* pawn, int x, int y)
{
    m_Grid [x + SIZE * y] = pawn;
}

void Board::AddPawn (Pawn* pawn)
{
    m_Grid [pawn->m_PositionX + SIZE * pawn->m_PositionY] = pawn;
}

void Board::RemovePawnAt (int x, int y)
{
    Pawn *pawnat = m_Grid [x + SIZE * y];

    if (!pawnat) return;
    
    m_Grid [x + SIZE * y] = nullptr;
    
    delete pawnat;
}

int Board::GetGold (Color faction)
{
    if (faction == Color::RED || faction == Color::BLUE) return (m_Gold [faction]);

    return (-1);
}

int Board::GetPlayerGold ()
{
    return (m_Gold [m_CurrentFaction]);
}

std::vector<Pawn *> Board::GetAttackablePawns (Pawn *attacker)
{
    std::vector<Pawn *> victims;

    Color victim_faction = (attacker->m_Faction == Color::RED) ? Color::BLUE : Color::RED;

    int x, y = -1;

    for (int i = 0; i < m_Size*m_Size; ++i)
    {
        if (m_Grid [i])
        {
            if (m_Grid [i]->m_Faction != victim_faction) continue;

            x = i % m_Size;
            y = i / m_Size;

            if (abs (attacker->m_PositionX + attacker->m_PositionY - (x + y)) != 1) continue;

            victims.push_back (m_Grid [i]);
        }
    }

    return (victims);
}

std::vector<Pawn *> Board::GetAllPawns ()
{
    std::vector <Pawn *> pawns;

    for (auto& pawn : m_Grid)
    {
        if (!pawn) continue;
        pawns.push_back (pawn);
    }

    return (pawns);
}

Color Board::GetPlayingFaction ()
{
    return (m_CurrentFaction);
}

void Board::AddPlayerGold (int gold)
{
    m_Gold [m_CurrentFaction] += gold;
}

void Board::RemovePlayerGold (int gold)
{
    m_Gold [m_CurrentFaction] = (m_Gold [m_CurrentFaction] - gold < 0) ? 0 : m_Gold [m_CurrentFaction] - gold;
}

int Board::GetSize ()
{
    return (m_Size);
}

void Board::NextPlayer ()
{
    if (m_CurrentFaction == Color::RED) m_CurrentFaction = Color::BLUE;
    else if (m_CurrentFaction == Color::BLUE) m_CurrentFaction = Color::RED;
}
