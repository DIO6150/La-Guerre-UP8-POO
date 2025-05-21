#include "board.hpp"
#include "common.hpp"
#include "pawn.hpp"
#include "globals.hpp"
#include "string_pretty.hpp"
#include "utils.hpp"

#include "json.hpp"

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sys/ioctl.h>

using json = nlohmann::json;

bool Common::Attack (Pawn *pawn)
{
    int choice = 0;
    Globals::Print ("k_attack_target_ask");

    std::vector<Pawn *> victims = Globals::GameBoard->GetAttackablePawns (pawn);

    if (victims.size () == 0)
    {
        Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_error_action_target_range", false)));
        return (false);
    }

    while (choice <= 0 || choice > (int) victims.size ())
    {
        Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_action_range_error", false), victims.size ()));
        return (false);
    }

    int cost = abs(victims [choice - 1]->m_PositionX - pawn->m_PositionX) + abs(victims [choice - 1]->m_PositionY - pawn->m_PositionY);

    if (cost > 1)
    {
        Globals::ActionLogs.push_back (Globals::GetTranslation ("k_action_attack_range_error", false));
        return (false);
    }

    pawn->Attack (victims [choice - 1]);

    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_log_action_attack", false), victims [choice - 1]->GetName (), victims [choice - 1]->m_PositionX, victims [choice - 1]->m_PositionY));
    return (true);
}

bool Common::Move (Pawn *pawn)
{
    auto v = getUserInputs ({
        {"int", {}, std::make_pair (0, Globals::GameBoard->GetSize ()-1)}, 
        {"int", {}, std::make_pair (0, Globals::GameBoard->GetSize ()-1)}},
    Globals::GetPrettyfiedTranslation ("k_ask_action_move_pos"));

    int old_x = pawn->m_PositionX;
    int old_y = pawn->m_PositionY;

    int x = std::get <int> (v[0]);
    int y = std::get <int> (v[1]);

    if (Globals::GameBoard->GetPawnAt (x, y))
    {
        Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_error_action_move_tile_occupied")));
        return (false);
    }

    if (pawn->m_RemainingMovement < abs(pawn->m_PositionX - x) + abs(pawn->m_PositionY - y))
    {
        Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_error_action_move_stamina")));
        return (false);
    }

    Globals::GameBoard->MovePawn (pawn, x, y);
    pawn->Move (x, y);
    
    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_log_action_move"), pawn->GetName (), old_x, old_y, x, y));
    // TODO : moving pawns from the Pawn::Move method should not move on the board too
    return (true);
}

bool Common::Transform (Pawn *pawn)
{
    Castle *castle = new Castle (pawn->m_Faction, pawn->m_PositionX, pawn->m_PositionY);

    Globals::GameBoard->RemovePawnAt (pawn->m_PositionX, pawn->m_PositionY);
    Globals::GameBoard->AddPawn (castle);

    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_log_action_transform"), castle->m_PositionX, castle->m_PositionY));
    return (true);
}

bool Common::CreatePawn (Pawn *pawn)
{
    /* GET PAWN POSITION */
    std::vector<std::string> cardinals = {
        Globals::GetTranslation ("k_icon_north"),
        Globals::GetTranslation ("k_icon_south"),
        Globals::GetTranslation ("k_icon_west"),
        Globals::GetTranslation ("k_icon_east")
    };

    auto user_input1 = getUserInputs ({{"string", cardinals, std::nullopt}}, Globals::GetPrettyfiedTranslation ("k_ask_action_create_pawn_pos"));
    std::string direction = std::get <std::string> (user_input1[0]);
    
    int x = 0;
    int y = 0;

    if      (direction == cardinals [0]) y = - 1;
    else if (direction == cardinals [1]) y = + 1;
    else if (direction == cardinals [2]) x = - 1;
    else if (direction == cardinals [3]) x = + 1;

    x += pawn->m_PositionX;
    y += pawn->m_PositionY;

    std::cout << x << " " << y << " " << direction << std::endl;

    if (x < 0 || x >= Globals::GameBoard->GetSize () || y < 0 || y >= Globals::GameBoard->GetSize ())
    {
        Globals::ActionLogs.push_back (Globals::GetPrettyfiedTranslation ("k_error_action_create_pawn_outside"));
        return (false);
    }

    if (Globals::GameBoard->GetPawnAt (x, y) != nullptr)
    {
        Globals::ActionLogs.push_back (Globals::GetPrettyfiedTranslation ("k_error_action_create_pawn_tile_occupied"));
        return (false);
    }

    /* GET PAWN TYPE */

    std::vector<std::string> pawn_types = {
        Globals::GetTranslation ("k_short_pawn_farmer"),
        Globals::GetTranslation ("k_short_pawn_warrior"),
        Globals::GetTranslation ("k_short_pawn_lord"),
        Globals::GetTranslation ("k_short_pawn_castle"),
    };

    
    std::vector<std::function<Pawn * ()>> factories = {
        [&pawn, &x, &y]() -> Pawn * { return (new Farmer (pawn->m_Faction, x, y)); },
        [&pawn, &x, &y]() -> Pawn * { return (new Warrior (pawn->m_Faction, x, y)); },
        [&pawn, &x, &y]() -> Pawn * { return (new Lord (pawn->m_Faction, x, y)); },
        [&pawn, &x, &y]() -> Pawn * { return (new Castle (pawn->m_Faction, x, y)); }
    };
    
    Pawn *new_pawn;

    auto user_input2 = getUserInputs ({{"string", pawn_types, std::nullopt}}, Globals::GetPrettyfiedTranslation ("k_ask_action_create_pawn_type"));
    std::string pawn_type = std::get <std::string> (user_input2[0]);

    if      (pawn_type == pawn_types [0]) new_pawn = factories [0] ();
    else if (pawn_type == pawn_types [1]) new_pawn = factories [1] ();
    else if (pawn_type == pawn_types [2]) new_pawn = factories [2] ();
    else if (pawn_type == pawn_types [3]) new_pawn = factories [3] ();

    if (new_pawn->m_GoldCost > Globals::GameBoard->GetGold (pawn->m_Faction))
    {
        delete new_pawn;
        
        Globals::ActionLogs.push_back (Globals::GetPrettyfiedTranslation ("k_error_action_create_pawn_poor"));
        return (false);
    }
    
    Globals::GameBoard->AddPawn (new_pawn);
    
    Globals::GameBoard->RemovePlayerGold (new_pawn->m_GoldCost);
    
    Globals::ActionLogs.push_back (Globals::GetPrettyfiedTranslation ("k_log_action_create_pawn", new_pawn->GetName (), new_pawn->m_PositionX, new_pawn->m_PositionY));
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
            std::cout << " " << Globals::GetTranslation ("k_icon_pawn_empty") << " ";
        }

        if (i != 0 && (i + 1) % Globals::GameBoard->GetSize () == 0) std::cout << std::endl;
    }
}

static std::vector <Action> GenerateActionList (bool& continue_turn)
{
    std::vector <Action> actions {};

    for (auto& pawn : Globals::GameBoard->GetAllPawns ())
    {
        if (pawn->m_Faction != Globals::GameBoard->GetPlayingFaction ()) continue;

        std::string pawn_info;

        pawn_info = Globals::GetPrettyfiedTranslation ("k_info_pawn", 
            pawn->GetIcon (), 
            pawn->GetName (),
            pawn->m_Health,
            pawn->m_PositionX,
            pawn->m_PositionY,
            pawn->m_RemainingMovement,
            pawn->m_MovementCost);

        actions.push_back ((Action){
            pawn_info, [pawn, pawn_info]() {
                int i = 0;

                auto actions = pawn->GetActionList ();
                actions.push_back ((Action) {
                    Globals::GetTranslation ("k_name_action_cancel"),
                    [] () -> int {
                        return (false);
                    }
                });
                
                if (actions.size () == 0)
                {
                    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_short_no_action")));
                    return (false);
                }
                
                
                for (auto& action : actions)
                {
                    Globals::Print ("k_common_list_element", ++i, action.name);
                }

                auto user_input = getUserInputs ({{"int", {}, std::make_pair(1, actions.size ()) }}, Globals::GetPrettyfiedTranslation ("k_ask_action"));
                int choice = std::get <int> (user_input[0]);

                actions [choice - 1].func ();
                return (true);
            }
        });
    }

    actions.push_back ((Action){
        Globals::GetTranslation ("k_name_action_end_turn"), [&continue_turn]() { continue_turn = false; return (true); }
    });

    actions.push_back ((Action){
        Globals::GetTranslation ("k_name_action_save"), []() {
            nlohmann::json data;

            data ["board"] = nlohmann::json::array ();

            for (auto &pawn : Globals::GameBoard->GetAllPawns ())
            {
                data ["board"].push_back ({
                    {"type", pawn->GetType ()},
                    {"position", {pawn->m_PositionX, pawn->m_PositionY}},
                    {"health", pawn->m_Health},
                    {"faction", pawn->m_Faction },
                    {"movement", pawn->m_RemainingMovement}
                });
            }

            data ["faction_turn"] = int (Globals::GameBoard->GetPlayingFaction ());

            data ["gold"] = {
                {Color::RED, Globals::GameBoard->GetGold (Color::RED)},
                {Color::BLUE, Globals::GameBoard->GetGold (Color::BLUE)}
            };

            std::fstream file {"save.json"};

            auto user_input = getUserInputs ({{"string", {}, std::nullopt}}, Globals::GetPrettyfiedTranslation ("k_ask_action_save_name"));
            std::string save_name = std::get<std::string> (user_input[0]);

            std::string line;
            std::string text_file;
            while (std::getline (file, line))
            {
                text_file += line;
            }

            file.close ();
            
            json save_data = {};

            if (text_file != "")
            {
                save_data = json::parse (text_file);
            }

            if (save_data.contains (save_name)) {
                user_input = getUserInputs ({{"string", {
                            Globals::GetTranslation ("k_short_yes"),
                            Globals::GetTranslation ("k_short_no")},
                        std::nullopt}},
                    Globals::GetPrettyfiedTranslation ("k_ask_action_save_save_overwrite"));

                std::string answer = std::get<std::string> (user_input[0]);

                if (answer == Globals::GetTranslation ("k_short_no"))
                {
                    Globals::ActionLogs.push_back (Globals::GetPrettyfiedTranslation ("k_cancel_action", Globals::GetTranslation ("k_name_action_save")));
                    return (false);
                }
            }

            save_data [save_name] = data;

            std::string data_str = save_data.dump ();
            
            file.open ("save.json");
            file << save_data.dump (4);
            file.close ();

            Globals::ActionLogs.push_back (Globals::GetPrettyfiedTranslation ("k_log_save_save_game"));

            return (true);
        }
    });

    actions.push_back ((Action){
        Globals::GetTranslation ("k_name_action_main_menu"), [&continue_turn]() {
            Globals::State = Globals::MENU;
            continue_turn = false;
            return (true);
        }
    });
    
    actions.push_back ((Action){
        Globals::GetTranslation ("k_name_action_quit"), [&continue_turn]() {
            Globals::State = Globals::QUITTING;
            continue_turn = false;
            return (true);
        }
    });

    return (actions);
}

static std::string get_playing_faction ()
{
    Color c = Globals::GameBoard->GetPlayingFaction ();

    return (Dye (Globals::GetTranslation (KeyFromColor (c)), c));
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

    if (gold_earned > 0)
        actions_log.push_back (StrPretty ( Globals::GetTranslation ("k_common_list_element"), 0, StrPretty (Globals::GetTranslation ("k_info_gold"), gold_earned).c_str ()));
    
    while (continue_turn)
    {
        ClearScreen ();
        PrintBoard ();
        Globals::Print ("k_info_faction", get_playing_faction ().c_str ());
        Globals::Print ("k_info_log_title");
        for (auto& l : actions_log)
        {
            Globals::PrintPretty (l);
        }
        Globals::Print ("k_info_log_empty");
        Globals::Print ("k_info_gold", Globals::GameBoard->GetPlayerGold ());

        actions = GenerateActionList (continue_turn);
        for (int i = 0; i < (int) actions.size (); ++i)
        {
            Globals::Print ("k_common_list_element", i + 1, actions [i].name.c_str ());
        }
        
        auto user_input = getUserInputs ({{"int", {}, std::make_pair(1, actions.size ()) }}, Globals::GetPrettyfiedTranslation ("k_ask_action"));
        int choice = std::get <int> (user_input[0]);

        while (true)
        {

            ClearScreen ();
            PrintBoard ();

            Globals::Print ("k_info_faction", get_playing_faction ());
            Globals::Print ("k_info_gold", Globals::GameBoard->GetPlayerGold ());
            Globals::Print ("k_info_choice", actions [choice - 1].name);

            actions [choice - 1].func ();

            int i = 0;
            for (auto& log : Globals::ActionLogs)
            {
                if (i) action_log += " > ";
                action_log += log;
                ++i;
            }

            if (i != 0) actions_log.push_back(StrPretty (Globals::GetTranslation ("k_common_list_element"), actions_log.size () + 1, action_log));

            action_log = "";
            Globals::ActionLogs.clear ();
            break;
        }
    }

    Globals::GameBoard->NextPlayer ();
}

void Common::Menu ()
{
    std::vector<Action> actions;

    actions.push_back ((Action){
        Globals::GetPrettyfiedTranslation ("k_name_action_new_game"),
        []() -> bool {
            Globals::State = Globals::GAME;
            // TODO : CREATE A REAL FUCKING BOAD OMG
            return (true);
        }
    });

    actions.push_back ((Action){
        Globals::GetPrettyfiedTranslation ("k_name_action_load"),
        []() -> bool {
            Globals::Print ("k_info_saves");

            if (LoadGame ())
            {
                Globals::State = Globals::GAME;
                return (true);
            }

            return (false);
        }
    });

    actions.push_back ((Action){
        Globals::GetPrettyfiedTranslation ("k_name_action_settings"),
        []() -> bool {
            Settings ();
            return (true);
        }
    });

    actions.push_back ((Action){
        Globals::GetPrettyfiedTranslation ("k_name_action_quit"),
        []() -> bool {
            Common::ClearScreen ();
            Globals::State = Globals::QUITTING;
            return (true);
        }
    });

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    ClearScreen ();
    Globals::Print ("k_title");

    for (int i = 0; i < w.ws_col; ++i) std::cout << "_";
    std::cout << std::endl;

    int i = 0;
    for (auto& a : actions)
    {
        Globals::Print ("k_common_list_element", ++i, a.name);
    }

    auto next_frame = getUserInputsNoErr ({{"int", {}, std::make_pair (1, actions.size ())}}, "");
    
    if (next_frame){
        for (int i = 0; i < w.ws_col; ++i) std::cout << "_";
        std::cout << std::endl;
        
        actions [std::get<int> ((*next_frame)[0]) - 1].func ();
    }
}

void Common::ClearScreen ()
{
    #ifdef __linux__
        system ("clear");
    #else
        system ("cls");
    #endif
}