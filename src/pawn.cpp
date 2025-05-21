#include "board.hpp"
#include "common.hpp"
#include "colors.hpp"
#include "globals.hpp"
#include "pawn.hpp"

#include <iostream>

Pawn::Pawn (Color faction, int x, int y, int attack, int health, int mv_cost, int gold_produced, int gold_cost):
m_Faction (faction), m_PositionX (x), m_PositionY (y), m_AttackPower (attack),
m_Health (health), m_MovementCost (mv_cost), m_RemainingMovement (mv_cost), m_GoldProduced (gold_produced), m_GoldCost (gold_cost), m_CanAttack (true)
{

}

Pawn::~Pawn ()
{

}

Color Pawn::GetFaction ()
{
    return (m_Faction);
}

bool Pawn::Attack (Pawn* pawn)
{
    pawn->m_Health -= m_AttackPower;
    return (true);
}

bool Pawn::Move (int x, int y)
{
    m_RemainingMovement -= abs(m_PositionX - x) + abs(m_PositionY - y);

    m_PositionX = x;
    m_PositionY = y;

    return (true);
}

void Pawn::BeginTurn ()
{
    
}
