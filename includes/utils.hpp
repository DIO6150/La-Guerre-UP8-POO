#pragma once

#include <functional>
#include <variant>
#include <vector>
#include <string>
#include <optional>

// Alias pour simplifier la gestion de types
using Argument = std::variant<int, float, std::string>;
using TypeChecker = std::function<std::optional<Argument>(const std::string&)>;

struct ArgSpec
{
    std::string type;                       // "int", "float", "string"
    std::vector<std::string> allowed;       // Si non vide, valeurs autorisées
    std::optional<std::pair<float, float>> range;
};

// Fonctions de conversion sécurisées
std::optional<Argument> tryParseInt(const std::string& token);

std::optional<Argument> tryParseFloat(const std::string& token);

std::optional<Argument> tryParseString(const std::string& token);

std::vector<Argument> getUserInputs(const std::vector<ArgSpec>& specs, std::string question);
std::optional<std::vector<Argument>> getUserInputsNoErr(const std::vector<ArgSpec>& specs, std::string question);

bool LoadGame ();
void Settings ();
