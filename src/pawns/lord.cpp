#include "board.hpp"
#include "common.hpp"
#include "colors.hpp"
#include "globals.hpp"
#include "pawn.hpp"

#include <iostream>

Lord::Lord (Color faction, int x, int y) : Pawn (faction, x, y, 3, 5, 1, 0, 10)
{

}

Lord::~Lord ()
{

}

// TODO : eventuellement tout foutre dans des fonctions "commons"
std::vector<Action> Lord::GetActionList ()
{
    return {
        {
            Globals::GetTranslation ("k_name_action_attack"), [this] () -> bool { return (Common::Attack (this)); }
        },
        {
            Globals::GetTranslation ("k_name_action_move"), [this] () -> bool { return (Common::Move (this)); }
        },
        {
            Globals::GetTranslation ("k_name_action_transform"), [this] () -> bool { return (Common::Transform (this)); }
        },
    };
}

std::string Lord::GetIcon ()
{
    return (Dye (Globals::GetTranslation ("k_icon_pawn_lord"), m_Faction));
}

std::string Lord::GetName ()
{
    return (Dye (Globals::GetTranslation ("k_name_pawn_lord"), m_Faction));
}

const std::string Lord::GetType () const
{
    return ("lord");
}