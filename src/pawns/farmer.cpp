#include "board.hpp"
#include "common.hpp"
#include "colors.hpp"
#include "globals.hpp"
#include "pawn.hpp"

#include <iostream>

Farmer::Farmer (Color faction, int x, int y) : Pawn (faction, x, y, 0, 1, 2, 5, 20)
{
    
}

Farmer::~Farmer ()
{

}

std::vector<Action> Farmer::GetActionList ()
{
    return {
        {
            Globals::GetTranslation ("k_name_action_move"), [this] () -> bool { return (Common::Move (this)); }
        }
    };
}

std::string Farmer::GetIcon ()
{
    return (Dye (Globals::GetTranslation ("k_icon_pawn_farmer"), m_Faction));
}

std::string Farmer::GetName ()
{
    return (Dye (Globals::GetTranslation ("k_name_pawn_farmer"), m_Faction));
}

const std::string Farmer::GetType () const
{
    return ("farmer");
}