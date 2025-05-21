#pragma once

#include <vector>
#include <functional>
#include <string>

#include "colors.hpp"
#include "action.hpp"

class Pawn
{
public:
    Color m_Faction;

    int m_PositionX;
    int m_PositionY;

    int m_AttackPower;
    int m_Health;
    int m_MovementCost;
    int m_RemainingMovement;
    int m_GoldProduced;
    int m_GoldCost;

    bool m_CanAttack;

    Pawn (Color faction, int x, int y, int attack, int health, int mv_cost, int gold_produced, int gold_cost);

    virtual ~Pawn () = 0;
    Color GetFaction ();

    virtual std::vector<Action> GetActionList () = 0;
    virtual std::string GetName () = 0;
    virtual std::string GetIcon () = 0;
    virtual const std::string GetType () const = 0;

    bool Attack (Pawn *pawn);
    bool Move (int x, int y);

    void BeginTurn ();
};

class Castle : public Pawn
{

public:
    Castle (Color faction, int x, int y);
    ~Castle ();
    std::vector<Action> GetActionList ();
    std::string GetName ();
    std::string GetIcon ();
    const std::string GetType () const;
};

class Lord : public Pawn
{
    
public:
    Lord (Color faction, int x, int y);
    ~Lord ();

    std::vector<Action> GetActionList ();
    std::string GetName ();
    std::string GetIcon ();
    const std::string GetType () const;
};

class Warrior : public Pawn
{

public:
    Warrior (Color faction, int x, int y);
    ~Warrior ();

    std::vector<Action> GetActionList ();
    std::string GetName ();
    std::string GetIcon ();
    const std::string GetType () const;
};

class Farmer : public Pawn
{

public:
    Farmer (Color faction, int x, int y);
    ~Farmer ();

    std::vector<Action> GetActionList ();
    std::string GetName ();
    std::string GetIcon ();
    const std::string GetType () const;
};
