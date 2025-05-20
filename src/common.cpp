#include "board.hpp"
#include "common.hpp"
#include "pawn.hpp"
#include "globals.hpp"
#include "string_pretty.hpp"

#include "json.hpp"

#include <stdlib.h>
#include <iostream>
#include <fstream>

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

    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_action_log_attack", false), victims [choice - 1]->GetName (), victims [choice - 1]->m_PositionX, victims [choice - 1]->m_PositionY));
    return (true);
}

bool Common::Move (Pawn *pawn)
{
    int x, y, old_x, old_y;
    Globals::Print ("k_ask_action_move_pos");

    old_x = pawn->m_PositionX;
    old_y = pawn->m_PositionY;

    std::cin >> x;
    std::cin >> y;

    if (x < 0 || x >= Globals::GameBoard->GetSize () || y < 0 || y >= Globals::GameBoard->GetSize ())
    {
        Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_action_move_outside_error")));
        return (false);
    }

    if (Globals::GameBoard->GetPawnAt (x, y))
    {
        Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_tile_full_error")));
        return (false);
    }

    if (pawn->m_RemainingMovement < abs(pawn->m_PositionX - x) + abs(pawn->m_PositionY - y))
    {
        Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_error_action_move_stamina")));
        return (false);
    }

    Globals::GameBoard->MovePawn (pawn, x, y);
    pawn->Move (x, y);
    
    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_action_log_move"), pawn->GetName ().c_str (), old_x, old_y, x, y));
    // TODO : moving pawns from the Pawn::Move method should not move on the board too
    return (true);
}

bool Common::Transform (Pawn *pawn)
{
    Castle *castle = new Castle (pawn->m_Faction, pawn->m_PositionX, pawn->m_PositionY);

    Globals::GameBoard->RemovePawnAt (pawn->m_PositionX, pawn->m_PositionY);
    Globals::GameBoard->AddPawn (castle);

    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_action_log_transform"), castle->m_PositionX, castle->m_PositionY));
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
        Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_input_unknown_char_error"), choice));
        return (false);
    }

    if (x < 0 || x >= Globals::GameBoard->GetSize () || y < 0 || y >= Globals::GameBoard->GetSize ())
    {
        Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_action_create_pawn_outside_error")));
        return (false);
    }

    if (Globals::GameBoard->GetPawnAt (x, y) != nullptr)
    {
        Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_tile_full_error")));
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
            
            Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_not_enought_gold_error")));
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
            
            Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_not_enought_gold_error")));
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
            
            Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_not_enought_gold_error")));
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
            
            Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_not_enought_gold_error")));
            return (false);
        }
        
        Globals::GameBoard->AddPawn ((Castle *) np);
    }
    
    else
    {
        Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_input_unknown_char_error"), choice));
        return (false);
    }

    Globals::GameBoard->RemovePlayerGold (np->m_GoldCost);
    
    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_action_log_create_pawn"), np->GetName ().c_str (), np->m_PositionX, np->m_PositionY));
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

static std::vector <Action> GenerateSettingsList ()
{
    std::vector<Action> actions;

    actions.push_back ((Action){
        Globals::GetTranslation ("k_name_action_language"),
        [] () -> int {
            Globals::Print ("k_name_action_lang_info");

            std::vector <Action> langs = {
                {
                    Globals::GetTranslation ("k_name_action_lang_fr"),
                    [] () -> int {
                        Globals::LoadLanguage ("lang/fr_fr.json");
                        return (true);
                    }
                }
                ,
                {
                    Globals::GetTranslation ("k_name_action_lang_en"),
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
                Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_action_range_error"), langs.size ()));
                return (false);
            }

            langs [--choice].func ();

            Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_action_log_lang_change"), langs [choice].name.c_str ()));
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

        pawn_info = StrPretty (Globals::GetTranslation ("k_info_pawn"), pawn->GetIcon ().c_str (), pawn->GetName ().c_str (), pawn->m_Health, pawn->m_PositionX, pawn->m_PositionY, pawn->m_RemainingMovement, pawn->m_MovementCost);

        actions.push_back ((Action){
            pawn_info, [pawn, pawn_info]() {
                int i = 0;
                int choice = -1;

                auto actions = pawn->GetActionList ();
                actions.push_back ((Action) {
                    Globals::GetTranslation ("k_name_action_cancel"),
                    [] () -> int {
                        return (false);
                    }
                });
                
                if (actions.size () == 0)
                {
                    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_no_action")));
                    return (false);
                }
                
                Globals::Print ("k_ask_action");

                for (auto& action : actions)
                {
                    Globals::Print ("k_common_list_element", ++i, action.name);
                }

                std::cin >> choice;

                while (choice < 0 || choice > (int) actions.size ())
                {
                    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_action_range_error", false), actions.size ()));
                    return (false);
                }

                actions [--choice].func ();
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


            std::string save_name;

            Globals::Print ("k_ask_save_save_name");
            std::cin >> save_name;

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
                std::string answer;

                Globals::Print ("k_ask_save_save_overwrite");
                std::cin >> answer;
                if (answer == Globals::GetTranslation ("k_yes"))
                {
                    
                }

                else if (answer == Globals::GetTranslation ("k_no"))
                {
                    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_cancel_action"), Globals::GetTranslation ("k_name_action_save").c_str ()));
                    return (false);
                }

                else
                {
                    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_error_input_invalid")));
                    return (false);
                }
            }

            save_data [save_name] = data;

            std::string data_str = save_data.dump ();

            std::cout << data_str << std::endl;
            
            file.open ("save.json");
            file << save_data.dump (4);
            file.close ();

            Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_log_saved_game")));

            return (true);
        }
    });

    actions.push_back ((Action){
        Globals::GetTranslation ("k_name_action_load"), []() {

            std::ifstream file {"save.json"};

            json save_data;

            std::string line;
            std::string text_file;
            while (std::getline (file, line))
            {
                text_file += line;
            }
            file.close ();

            if (text_file == "")
            {
                Globals::ActionLogs.push_back (StrPretty(Globals::GetTranslation ("k_error_no_saved_data")));
                return (false);
            }

            save_data = json::parse (text_file);

            std::vector <json> datas;

            int i = 0;
            for (auto& [name, data] : save_data.items ())
            {
                datas.push_back (data);
                Globals::Print ("k_common_list_element", ++i, name.c_str ());
            }

            int save_index;
            Globals::Print ("k_ask_save_load_index");
            std::cin >> save_index;

            Globals::GameBoard->Clear ();

            json data = datas [i-1];
            for (auto& pawn : data ["board"])
            {
                Pawn *pawn_to_add;

                if (pawn ["type"] == "farmer")
                {
                    pawn_to_add = new Farmer (pawn ["faction"], pawn ["position"][0], pawn ["position"][1]);
                }

                else if (pawn ["type"] == "warrior")
                {
                    pawn_to_add = new Warrior (pawn ["faction"], pawn ["position"][0], pawn ["position"][1]);
                }

                else if (pawn ["type"] == "lord")
                {
                    pawn_to_add = new Lord (pawn ["faction"], pawn ["position"][0], pawn ["position"][1]);
                }

                else if (pawn ["type"] == "castle")
                {
                    pawn_to_add = new Castle (pawn ["faction"], pawn ["position"][0], pawn ["position"][1]);
                }

                pawn_to_add->m_Health = pawn ["health"];
                pawn_to_add->m_RemainingMovement = pawn ["movement"];
                Globals::GameBoard->AddPawn (pawn_to_add);
            }
            
            Globals::GameBoard->SetGold (Color (data ["gold"][0][0]), data ["gold"][0][1]);
            Globals::GameBoard->SetGold (Color (data ["gold"][1][0]), data ["gold"][1][1]);

            Globals::GameBoard->SetFactionPlaying (Color (data ["faction_turn"]));

            Globals::ActionLogs.clear ();

            Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_log_load_game")));

            return (true);
        }
    });

    actions.push_back ((Action){
        Globals::GetTranslation ("k_name_action_settings"), []() { 
            Common::ClearScreen ();

            Globals::Print ("k_name_action_settings");

            std::vector<Action> actions = GenerateSettingsList ();

            actions.push_back ((Action) {
                Globals::GetTranslation ("k_name_action_cancel"),
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
                Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_action_range_error"), actions.size ()));
                return (false);
            }

            actions [--choice].func ();
            return (true);
        }
    });

    actions.push_back ((Action){
        Globals::GetTranslation ("k_name_action_quit"), [&continue_turn]() {
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
        actions_log.push_back (StrPretty ( Globals::GetTranslation ("k_log_list"), 0, StrPretty (Globals::GetTranslation ("k_log_gold"), gold_earned).c_str ()));
    
    while (continue_turn)
    {
        ClearScreen ();
        PrintBoard ();
        Globals::Print ("k_info_faction", get_playing_faction ().c_str ());
        Globals::Print ("k_log_title_info");
        for (auto& l : actions_log)
        {
            Globals::PrintPretty (l);
        }
        Globals::Print ("k_log_empty_info");
        Globals::Print ("k_info_gold", Globals::GameBoard->GetPlayerGold ());
        Globals::Print ("k_info_choice");

        actions = GenerateActionList (continue_turn);
        for (int i = 0; i < (int) actions.size (); ++i)
        {
            Globals::Print ("k_common_list_element", i + 1, actions [i].name.c_str ());
        }
        Globals::Print ("k_ask_action");

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

            if (i != 0) actions_log.push_back(StrPretty (Globals::GetTranslation ("k_log_list"), actions_log.size (), action_log.c_str ()));

            action_log = "";
            Globals::ActionLogs.clear ();
            break;
        }
    }

    Globals::GameBoard->NextPlayer ();
}

void Common::ClearScreen ()
{
    return;
    #ifdef __linux__
        system ("clear");
    #else
        system ("cls");
    #endif
}