#include "board.hpp"
#include "common.hpp"
#include "colors.hpp"
#include "globals.hpp"
#include "pawn.hpp"
#include "string_color.hpp"

#include <iostream>

Warrior::Warrior (Color faction, int x, int y) : Pawn (faction, x, y, 5, 10, 3, 0, 10)
{

}

Warrior::~Warrior ()
{

}

std::vector<Action> Warrior::GetActionList ()
{
    return {
        {
            Globals::GetTranslation ("k_action_name_attack"), [this] () -> bool { return (Common::Attack (this)); }
        },
        {
            Globals::GetTranslation ("k_action_name_move"), [this] () -> bool { return (Common::Move (this)); }
        }
    };
}

std::string Warrior::GetIcon ()
{
    return (Dye (Globals::GetTranslation ("k_pawn_icon_warrior"), m_Faction));
}

std::string Warrior::GetName ()
{
    return (Dye (Globals::GetTranslation ("k_pawn_name_warrior"), m_Faction));
}