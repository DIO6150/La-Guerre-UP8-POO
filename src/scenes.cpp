#include "scenes.hpp"

#include "globals.hpp"
#include "board.hpp"
#include "action.hpp"
#include "string_pretty.hpp"
#include "pawn.hpp"
#include "common.hpp"
#include "utils.hpp"

#include <vector>
#include <string>

void Play ()
{
    std::vector <Action> actions;
    std::vector <std::string> actions_log;
    std::string action_log;
    
    bool continue_turn = true;

    // Hacky way of spawning gold only after the first 2 turns (RED then BLUE) Modify 2 if want more players
    if (Globals::FirstGold >= 2)
    {
        // Calculate gold earned at begining of turn 
        int gold_earned = 0;
        for (auto& pawn : Globals::GameBoard->GetAllPawns ())
        {
            if (pawn->m_Faction != Globals::GameBoard->GetPlayingFaction ()) continue;
            gold_earned += pawn->m_GoldProduced;

            // The pawns are now revigorated !
            pawn->m_RemainingMovement = pawn->m_MovementCost;
            pawn->m_CanAttack = true;
        }
        Globals::GameBoard->AddPlayerGold (gold_earned);
        
        if (gold_earned > 0)
        actions_log.push_back (Globals::GetPrettyfiedTranslation ("k_common_list_element", 1, Globals::GetPrettyfiedTranslation ("k_log_action_end_gold", gold_earned)));
    }
    
    Globals::FirstGold ++;

    while (continue_turn)
    {
        Common::ClearScreen ();
        Common::PrintBoard ();
        // Print history of previous actions this turn
        Globals::Print ("k_info_faction", Common::GetColoredPlayingFaction ());
        Globals::Print ("k_info_log_title");
        for (auto& l : actions_log) Globals::PrintPretty (l);
        Globals::Print ("k_info_log_empty");
        Globals::Print ("k_info_gold", Globals::GameBoard->GetPlayerGold ());
        // Print the list of possible actions
        actions = Common::GeneratePlayingActions (continue_turn);
        for (int i = 0; i < (int) actions.size (); ++i) Globals::Print ("k_common_list_element", i + 1, actions [i].name);
       
        

        // Ask the user for the index of the action to play
        auto user_input = getUserInputs ({{"int", {}, std::make_pair(1, actions.size ()) }}, Globals::GetPrettyfiedTranslation ("k_ask_action"));
        int choice = std::get <int> (user_input[0]) - 1;



        // Print a truncated version of the main play screen with bare informations
        Common::ClearScreen ();
        Common::PrintBoard ();
        Globals::Print ("k_info_faction", Common::GetColoredPlayingFaction ());
        Globals::Print ("k_info_gold", Globals::GameBoard->GetPlayerGold ());
        Globals::Print ("k_info_choice", actions [choice].name);



        actions [choice].func ();


        // Generate a string of actions that happened on this particular "choice" (very rare)
        int i = 0;
        for (auto& log : Globals::ActionLogs)
        {
            if (i) action_log += " > ";
            action_log += log;
            ++i;
        }
        if (i != 0) actions_log.push_back (Globals::GetPrettyfiedTranslation ("k_common_list_element", actions_log.size () + 1, action_log));



        action_log = "";
        Globals::ActionLogs.clear ();
    }

    if (Globals::State == Globals::GAME) Globals::GameBoard->NextPlayer ();
}

void Menu ()
{
    std::vector<Action> actions;

    if (Globals::CanContinue)
    {
        actions.push_back ((Action){
            Globals::GetPrettyfiedTranslation ("k_name_action_continue"),
            []() -> bool {
                Globals::State = Globals::GAME;
                return (true);
            }
        });
    }

    actions.push_back ((Action){
        Globals::GetPrettyfiedTranslation ("k_name_action_new_game"),
        []() -> bool {
            Globals::State = Globals::GAME;
            Globals::CanContinue = true;
            Common::NewGame ();
            return (true);
        }
    });

    actions.push_back ((Action){
        Globals::GetPrettyfiedTranslation ("k_name_action_load"),
        []() -> bool {
            Globals::Print ("k_info_saves");

            if (Common::LoadGame ())
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
            Common::Settings ();
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

    Common::ClearScreen ();
    Globals::Print ("k_title");

    for (int i = 0; i < Common::GetConsoleColumns (); ++i) std::cout << "_";
    std::cout << std::endl;

    int i = 0;
    for (auto& a : actions)
    {
        Globals::Print ("k_common_list_element", ++i, a.name);
    }

    auto next_frame = getUserInputsNoErr ({{"int", {}, std::make_pair (1, actions.size ())}}, "");
    
    if (next_frame){
        for (int i = 0; i < Common::GetConsoleColumns (); ++i) std::cout << "_";
        std::cout << std::endl;
        
        actions [std::get<int> ((*next_frame)[0]) - 1].func ();
    }
}