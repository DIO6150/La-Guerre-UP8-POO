#include "utils.hpp"

#include "json.hpp"
#include "globals.hpp"
#include "board.hpp"
#include "pawn.hpp"
#include "common.hpp"

#include <iostream>
#include <sstream>
#include <limits>
#include <fstream>

using json = nlohmann::json;

std::optional<Argument> tryParseInt(const std::string& token)
{
    try
    {
        size_t idx;
        int value = std::stoi(token, &idx);
        if (idx == token.size()) return value;
    }
    catch (...) {}

    return (std::nullopt);
}

std::optional<Argument> tryParseFloat(const std::string& token)
{
    try
    {
        size_t idx;
        float value = std::stof(token, &idx);
        if (idx == token.size()) return value;
    }
    catch (...) {}

    return (std::nullopt);
}

std::optional<Argument> tryParseString(const std::string& token)
{
    return (token);
}

std::vector<Argument> getUserInputs(const std::vector<ArgSpec>& specs, std::string question) {
    std::vector<Argument> results;
    std::string line;

    while (true) {
        results.clear();
        std::cout << question << std::endl;

        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token) tokens.push_back(token);

        if (tokens.size() != specs.size()) {
            Globals::Print ("k_error_input_wrong_arg_count");
            continue;
        }

        bool valid = true;

        for (size_t i = 0; i < specs.size(); ++i) {
            const auto& spec = specs[i];
            const auto& raw = tokens[i];

            // Check allowed values if specified
            if (!spec.allowed.empty()) {
                if (std::find(spec.allowed.begin(), spec.allowed.end(), raw) == spec.allowed.end()) {
                    Globals::Print ("k_error_input_illegal_argument", raw, i + 1);
                    valid = false;
                    break;
                }
                results.push_back(raw); // Force string storage
                continue;
            }

            // Parse based on type
            std::optional<Argument> parsed;
            if (spec.type == "int") parsed = tryParseInt (raw);
            else if (spec.type == "float") parsed = tryParseFloat (raw);
            else if (spec.type == "string") parsed = tryParseString (raw);
            else {
                Globals::Print ("k_error_input_unsupported_type", spec.type);
                valid = false;
                break;
            }

            if (!parsed) {
                Globals::Print ("k_error_input_wrong_arg_type", tokens[i], spec.type);
                valid = false;
                break;
            }

            if (spec.range.has_value()) {
                float min = spec.range->first;
                float max = spec.range->second;

                bool out_of_range = std::visit(
                    [&min, &max](auto&& val) -> bool {
                        using T = std::decay_t<decltype(val)>;

                        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>)
                        {
                            float f = static_cast<float>(val);
                            return f < min || f > max;
                        }

                        else
                        {
                            return true; // non numérique => invalide pour un champ à plage
                        }
                    },
                *parsed);

                if (out_of_range) {
                    Globals::Print ("k_error_input_range", min, max);
                    continue;
                }
            }

            results.push_back(*parsed);
        }

        if (valid) break;
    }

    return (results);
}

std::optional<std::vector<Argument>> getUserInputsNoErr(const std::vector<ArgSpec>& specs, std::string question)
{
    std::vector<Argument> results;
    std::string line;

    while (true) {
        results.clear();
        std::cout << question << std::endl;

        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token) tokens.push_back(token);

        if (tokens.size() != specs.size()) {
            return (std::nullopt);
        }

        bool valid = true;

        for (size_t i = 0; i < specs.size(); ++i) {
            const auto& spec = specs[i];
            const auto& raw = tokens[i];

            // Check allowed values if specified
            if (!spec.allowed.empty()) {
                if (std::find(spec.allowed.begin(), spec.allowed.end(), raw) == spec.allowed.end()) {
                    return (std::nullopt);
                }
                results.push_back(raw); // Force string storage
                continue;
            }

            // Parse based on type
            std::optional<Argument> parsed;
            if (spec.type == "int") parsed = tryParseInt (raw);
            else if (spec.type == "float") parsed = tryParseFloat (raw);
            else if (spec.type == "string") parsed = tryParseString (raw);
            else {
                return (std::nullopt);
            }

            if (!parsed) {
                return (std::nullopt);
            }

            if (spec.range.has_value()) {
                float min = spec.range->first;
                float max = spec.range->second;

                bool out_of_range = std::visit(
                    [&min, &max](auto&& val) -> bool {
                        using T = std::decay_t<decltype(val)>;

                        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float>)
                        {
                            float f = static_cast<float>(val);
                            return f < min || f > max;
                        }

                        else
                        {
                            return true; // non numérique => invalide pour un champ à plage
                        }
                    },
                *parsed);

                if (out_of_range) {
                    return (std::nullopt);
                }
            }

            results.push_back(*parsed);
        }

        if (valid) break;
    }

    return (results);
}

bool LoadGame ()
{
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
        Globals::ActionLogs.push_back (Globals::GetPrettyfiedTranslation ("k_error_no_saved_data"));
        return (false);
    }

    save_data = json::parse (text_file);

    std::unordered_map<std::string, json> datas;
    std::vector<std::string> names;

    int i = 0;
    for (auto& [name, data] : save_data.items ())
    {
        datas [name] = data;
        names.push_back (name);
        Globals::Print ("k_common_list_element", ++i, name);
    }
    
    auto user_input = getUserInputs ({{"string", names, std::nullopt}}, Globals::GetPrettyfiedTranslation ("k_ask_save_load_name"));
    std::string answer = std::get<std::string> (user_input[0]);

    Globals::GameBoard->Clear ();

    json data = datas [answer];
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

    Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_log_save_load_game")));

    return (true);
}

static std::vector <Action> GenerateSettingsList ()
{
    std::vector<Action> actions;

    actions.push_back ((Action){
        Globals::GetTranslation ("k_name_action_language"),
        [] () -> int {
            Globals::Print ("k_info_langs");

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
                Globals::Print ("k_common_list_element", i, lang.name);
                ++i;
            }

            auto next_frame = getUserInputs({{"int", {}, std::make_pair (1, langs.size ())}}, "");
            int choice = std::get<int> (next_frame[0]) - 1;

            langs [choice].func ();

            Globals::ActionLogs.push_back (StrPretty (Globals::GetTranslation ("k_log_action_lang_change"), langs [choice].name));
            return (true);
        }
    });

    return (actions);
}

void Settings ()
{
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
        Globals::Print ("k_common_list_element", i++, action.name);
    }

    auto next_frame = getUserInputs({{"int", {}, std::make_pair (1, actions.size ())}}, "");
    int choice = std::get<int> (next_frame[0]) - 1;

    actions [choice].func ();
}