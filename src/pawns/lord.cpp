#include "board.hpp"
#include "common.hpp"
#include "colors.hpp"
#include "globals.hpp"
#include "pawn.hpp"
#include "string_color.hpp"

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
            Globals::GetTranslation ("k_action_name_attack"), [this] () -> bool { return (Common::Attack (this)); }
        },
        {
            Globals::GetTranslation ("k_action_name_move"), [this] () -> bool { return (Common::Move (this)); }
        },
        {
            Globals::GetTranslation ("k_action_name_transform"), [this] () -> bool { return (Common::Transform (this)); }
        },
    };
}

std::string Lord::GetIcon ()
{
    return (Dye (Globals::GetTranslation ("k_pawn_icon_lord"), m_Faction));
}

std::string Lord::GetName ()
{
    return (Dye (Globals::GetTranslation ("k_pawn_name_lord"), m_Faction));
}
