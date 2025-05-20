#include "board.hpp"
#include "common.hpp"
#include "colors.hpp"
#include "globals.hpp"
#include "pawn.hpp"

#include <iostream>

Castle::Castle (Color faction, int x, int y): Pawn (faction, x, y, 0, 20, 0, 2, 15)
{

}

Castle::~Castle ()
{

}

std::vector<Action> Castle::GetActionList ()
{
    return {
        {
            Globals::GetTranslation ("k_name_action_create_pawn"), [this] () -> bool { return (Common::CreatePawn (this)); }
        }
    };
}

std::string Castle::GetIcon ()
{
    return (Dye (Globals::GetTranslation ("k_icon_pawn_castle"), m_Faction));
}

std::string Castle::GetName ()
{
    return (Dye (Globals::GetTranslation ("k_name_pawn_castle"), m_Faction));
}

const std::string Castle::GetType () const
{
    return ("castle");
}