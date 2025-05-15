#include "board.hpp"
#include "common.hpp"
#include "pawn.hpp"
#include "string_color.hpp"
#include "globals.hpp"

#include <stdlib.h>
#include <iostream>

// TODO : les faire retourtner un code d'erreur plutot 

bool Common::Attack (Pawn *pawn)
{
    int choice = 0;
    Globals::Print ("k_attack_target_ask");

    std::vector<Pawn *> victims = Globals::GameBoard->GetAttackablePawns (pawn);

    if (victims.size () == 0)
    {
        Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_target_range_error")));
        return (false);
    }

    while (choice <= 0 || choice > (int) victims.size ())
    {
        Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_range_error"), victims.size ()));
        return (false);
    }

    int cost = abs(victims [choice - 1]->m_PositionX - pawn->m_PositionX) + abs(victims [choice - 1]->m_PositionY - pawn->m_PositionY);

    if (cost > 1)
    {
        Globals::ActionLogs.push_back (Globals::GetTranslation ("k_action_attack_range_error"));
        return (false);
    }

    pawn->Attack (victims [choice - 1]);

    Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_log_attack"), victims [choice - 1]->GetName ().c_str (), victims [choice - 1]->m_PositionX, victims [choice - 1]->m_PositionY));
    return (true);
}

bool Common::Move (Pawn *pawn)
{
    int x, y, old_x, old_y;
    Globals::Print ("k_move_pos_ask");

    old_x = pawn->m_PositionX;
    old_y = pawn->m_PositionY;

    std::cin >> x;
    std::cin >> y;

    if (x < 0 || x >= Globals::GameBoard->GetSize () || y < 0 || y >= Globals::GameBoard->GetSize ())
    {
        Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_move_outside_error")));
        return (false);
    }

    if (Globals::GameBoard->GetPawnAt (x, y))
    {
        Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_tile_full_error")));
        return (false);
    }

    if (pawn->m_RemainingMovement < abs(pawn->m_PositionX - x) + abs(pawn->m_PositionY - y))
    {
        Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_move_not_enought_stamina")));
        return (false);
    }

    Globals::GameBoard->MovePawn (pawn, x, y);
    pawn->Move (x, y);
    
    Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_log_move"), pawn->GetName ().c_str (), old_x, old_y, x, y));
    // TODO : moving pawns from the Pawn::Move method should not move on the board too
    return (true);
}

bool Common::Transform (Pawn *pawn)
{
    Castle *castle = new Castle (pawn->m_Faction, pawn->m_PositionX, pawn->m_PositionY);

    Globals::GameBoard->RemovePawnAt (pawn->m_PositionX, pawn->m_PositionY);
    Globals::GameBoard->AddPawn (castle);

    Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_log_transform"), castle->m_PositionX, castle->m_PositionY));
    return (true);
}

bool Common::CreatePawn (Pawn *pawn)
{
    char choice;
    Globals::Print ("k_create_pawn_pos_ask");

    std::cin >> choice;

    int x, y;

    char north_letter = Globals::GetTranslation ("k_north_icon")[0];
    char south_letter = Globals::GetTranslation ("k_south_icon")[0];
    char west_letter = Globals::GetTranslation ("k_west_icon")[0];
    char east_letter = Globals::GetTranslation ("k_east_icon")[0];

    if (choice == north_letter)
    {
        x = pawn->m_PositionX;
        y = pawn->m_PositionY - 1;
    }

    else if (choice == south_letter)
    {
        x = pawn->m_PositionX;
        y = pawn->m_PositionY + 1;
    }

    else if (choice == west_letter)
    {
        x = pawn->m_PositionX - 1;
        y = pawn->m_PositionY;
    }
    
    else if (choice == east_letter)
    {
        x = pawn->m_PositionX + 1;
        y = pawn->m_PositionY;
    }

    else
    {
        std::cout << choice << std::endl;
        Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_input_unknown_char_error"), choice));
        return (false);
    }

    if (x < 0 || x >= Globals::GameBoard->GetSize () || y < 0 || y >= Globals::GameBoard->GetSize ())
    {
        Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_create_pawn_outside_error")));
        return (false);
    }

    if (Globals::GameBoard->GetPawnAt (x, y) != nullptr)
    {
        Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_tile_full_error")));
        return (false);
    }

    Globals::Print ("k_create_pawn_type_ask");

    choice = 0;
    
    std::cin >> choice;
    
    Pawn *np;
    
    
    char f_letter = Globals::GetTranslation ("k_pawn_short_farmer")[0];
    char w_letter = Globals::GetTranslation ("k_pawn_short_warrior")[0];
    char l_letter = Globals::GetTranslation ("k_pawn_short_lord")[0];
    char c_letter = Globals::GetTranslation ("k_pawn_short_castle")[0];
    
    
    if (choice == f_letter)
    {
        np = new Farmer {pawn->m_Faction, x, y};
        if (np->m_GoldCost > Globals::GameBoard->GetGold (pawn->m_Faction))
        {
            delete np;
            
            Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_not_enought_gold_error")));
            return (false);
        }
        
        Globals::GameBoard->AddPawn ((Farmer *) np);
    }
    
    else if (choice == w_letter)
    {
        np = new Warrior {pawn->m_Faction, x, y};
        if (np->m_GoldCost > Globals::GameBoard->GetGold (pawn->m_Faction))
        {
            delete np;
            
            Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_not_enought_gold_error")));
            return (false);
        }
        
        Globals::GameBoard->AddPawn ((Warrior *) np);
    }
    
    else if (choice == l_letter)
    {
        np = new Lord {pawn->m_Faction, x, y};
        if (np->m_GoldCost > Globals::GameBoard->GetGold (pawn->m_Faction))
        {
            delete np;
            
            Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_not_enought_gold_error")));
            return (false);
        }
        
        Globals::GameBoard->AddPawn ((Lord *) np);
    }
    
    else if (choice == c_letter)
    {
        np = new Castle {pawn->m_Faction, x, y};
        if (np->m_GoldCost > Globals::GameBoard->GetGold (pawn->m_Faction))
        {
            delete np;
            
            Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_not_enought_gold_error")));
            return (false);
        }
        
        Globals::GameBoard->AddPawn ((Castle *) np);
    }
    
    else
    {
        Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_input_unknown_char_error"), choice));
        return (false);
    }

    Globals::GameBoard->RemovePlayerGold (np->m_GoldCost);
    
    Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_log_create_pawn"), np->GetName ().c_str (), np->m_PositionX, np->m_PositionY));
    return (true);
}

void Common::PrintBoard ()
{
    std::cout << "      ";
    for (int i = 0; i < Globals::GameBoard->GetSize (); ++i)
    {
        std::cout << (i < 10 ? "  " : " ") << i;
    }

    std::cout << std::endl << std::endl;

    Pawn *pawn = nullptr;

    for (int i = 0; i < Globals::GameBoard->GetSize () * Globals::GameBoard->GetSize (); ++i)
    {
        if (i % Globals::GameBoard->GetSize () == 0)
        {
            std::cout << (i / Globals::GameBoard->GetSize () < 10 ? "  " : " ") << i / Globals::GameBoard->GetSize () << "   ";
        }

        pawn = Globals::GameBoard->GetPawnAt (i % Globals::GameBoard->GetSize (), i / Globals::GameBoard->GetSize ());

        if (pawn)
        {
            std::cout << " " << pawn->GetIcon () << " ";
        }
        else
        {
            std::cout << " " << Globals::GetTranslation ("k_pawn_icon_empty") << " ";
        }

        if (i != 0 && (i + 1) % Globals::GameBoard->GetSize () == 0) std::cout << std::endl;
    }
}

static std::vector <Action> GenerateSettingsList ()
{
    std::vector<Action> actions;

    actions.push_back ((Action){
        Globals::GetTranslation ("k_action_name_language"),
        [] () -> int {
            Globals::Print ("k_action_name_lang_info");

            std::vector <Action> langs = {
                {
                    Globals::GetTranslation ("k_action_name_lang_fr"),
                    [] () -> int {
                        Globals::LoadLanguage ("lang/fr_fr.json");
                        return (true);
                    }
                }
                ,
                {
                    Globals::GetTranslation ("k_action_name_lang_en"),
                    [] () -> int {
                        Globals::LoadLanguage ("lang/en_us.json");
                        return (true);
                    }
                }
            };

            int i = 1;
            for (auto& lang : langs)
            {
                Globals::Print ("k_common_option", i, lang.name.c_str ());
                ++i;
            }

            int choice = 0;

            std::cin >> choice;

            if (choice <= 0 || choice > (int) langs.size ())
            {
                Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_range_error"), langs.size ()));
                return (false);
            }

            langs [--choice].func ();

            Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_log_lang_change"), langs [choice].name.c_str ()));
            return (true);
        }
    });

    return (actions);
}

static std::vector <Action> GenerateActionList (bool& continue_turn)
{
    std::vector <Action> actions {};

    for (auto& pawn : Globals::GameBoard->GetAllPawns ())
    {
        if (pawn->m_Faction != Globals::GameBoard->GetPlayingFaction ()) continue;

        std::string pawn_info;

        pawn_info = StrColor (Globals::GetTranslation ("k_pawn_info"), pawn->GetIcon ().c_str (), pawn->GetName ().c_str (), pawn->m_Health, pawn->m_PositionX, pawn->m_PositionY, pawn->m_RemainingMovement, pawn->m_MovementCost);

        actions.push_back ((Action){
            pawn_info, [pawn, pawn_info]() {
                int i = 0;
                int choice = -1;

                auto actions = pawn->GetActionList ();
                actions.push_back ((Action) {
                    Globals::GetTranslation ("k_action_name_cancel"),
                    [] () -> int {
                        return (false);
                    }
                });
                
                if (actions.size () == 0)
                {
                    Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_no_action")));
                    return (false);
                }
                
                Globals::Print ("k_action_ask");

                for (auto& action : actions)
                {
                    Globals::Print ("k_action_list_info", ++i, action.name.c_str ());
                }

                std::cin >> choice;

                while (choice < 0 || choice > (int) actions.size ())
                {
                    Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_range_error"), actions.size ()));
                    return (false);
                }

                actions [--choice].func ();
                return (true);
            }
        });
    }

    actions.push_back ((Action){
        Globals::GetTranslation ("k_action_name_end_turn"), [&continue_turn]() { continue_turn = false; return (true); }
    });

    actions.push_back ((Action){
        Globals::GetTranslation ("k_action_name_save"), []() { return (true); }
    });

    actions.push_back ((Action){
        Globals::GetTranslation ("k_action_name_load"), []() { return (true); }
    });

    actions.push_back ((Action){
        Globals::GetTranslation ("k_action_name_settings"), []() { 
            Common::ClearScreen ();

            Globals::Print ("k_action_name_settings");

            std::vector<Action> actions = GenerateSettingsList ();

            actions.push_back ((Action) {
                Globals::GetTranslation ("k_action_name_cancel"),
                [] () -> int {
                    return (false);
                }
            });

            int i = 1;
            for (auto& action : actions)
            {
                Globals::Print ("k_common_option", i, action.name.c_str ());
                ++i;
            }


            int choice = 0;

            std::cin >> choice;

            if (choice <= 0 || choice > (int) actions.size ())
            {
                Globals::ActionLogs.push_back (StrColor (Globals::GetTranslation ("k_action_range_error"), actions.size ()));
                return (false);
            }

            actions [--choice].func ();
            return (true);
        }
    });

    actions.push_back ((Action){
        Globals::GetTranslation ("k_action_name_quit"), [&continue_turn]() {
            Globals::play = false;
            continue_turn = false;
            return (true);
        }
    });

    return (actions);
}

static std::string get_playing_faction ()
{
    Color c = Globals::GameBoard->GetPlayingFaction ();

    return (Dye (Globals::GetTranslation (StringFromColor (c)), c));
}

void Common::Play ()
{
    std::vector <Action> actions;
    std::vector <std::string> actions_log;
    std::string action_log;
    
    bool continue_turn = true;

    int gold_earned = 0;
    for (auto& pawn : Globals::GameBoard->GetAllPawns ())
    {
        if (pawn->m_Faction != Globals::GameBoard->GetPlayingFaction ()) continue;
        gold_earned += pawn->m_GoldProduced;
    }

    Globals::GameBoard->AddPlayerGold (gold_earned);

    if (gold_earned > 0) actions_log.push_back (StrColor ( Globals::GetTranslation ("k_log_list"), 0, StrColor (Globals::GetTranslation ("k_log_earned_gold"), gold_earned).c_str ()));
    
    while (continue_turn)
    {
        ClearScreen ();
        PrintBoard ();
        Globals::Print ("k_faction_turn", get_playing_faction ().c_str ());
        Globals::Print ("k_log_title_info");
        for (auto& e : actions_log)
        {
            Globals::Print ("k_free", e.c_str ());
        }
        Globals::Print ("k_log_empty_info");
        Globals::Print ("k_gold_info", Globals::GameBoard->GetPlayerGold ());
        Globals::Print ("k_possible_actions_info");

        actions = GenerateActionList (continue_turn);
        for (int i = 0; i < (int) actions.size (); ++i)
        {
            Globals::Print ("k_action_list_info", i + 1, actions [i].name.c_str ());
        }
        Globals::Print ("k_action_ask");

        int choice = 0;
        while (true)
        {
            std::cin >> choice;

            if (choice <= 0 || choice > (int) actions.size ())
            {
                Globals::Print ("k_invalid_choice");
                continue;
            }

            ClearScreen ();
            PrintBoard ();

            Globals::Print ("k_faction_turn", get_playing_faction ().c_str ());
            Globals::Print ("k_gold_info", Globals::GameBoard->GetPlayerGold ());
            Globals::Print ("k_action_choice_info", actions [choice - 1].name.c_str ());

            actions [choice - 1].func ();

            int i = 0;
            for (auto& log : Globals::ActionLogs)
            {
                if (i) action_log += " > ";
                action_log += log;
                ++i;
            }

            if (i != 0) actions_log.push_back(StrColor (Globals::GetTranslation ("k_log_list"), actions_log.size (), action_log.c_str ()));

            action_log = "";
            Globals::ActionLogs.clear ();
            break;
        }
    }

    Globals::GameBoard->NextPlayer ();
}

void Common::ClearScreen ()
{
    #ifdef __linux__
        system ("clear");
    #else
        system ("cls");
    #endif
}