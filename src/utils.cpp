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
                    valid = false;
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
