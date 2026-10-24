#pragma once
#include <string>

enum class TypeCondition { DegatsSubis, DegatsInfliges, CompetencesEpiques, CoupsDoubles, PvBas };

struct Condition {
    TypeCondition type;
    int seuil;
};

struct Equipement {
    std::string nom;
    int bonusPuissance;
    int bonusPv;
    int bonusMana;
    bool epique = false;
    Condition condition = {TypeCondition::DegatsSubis, 0};
};

struct Verrouille {
    Equipement objet;
    bool estArme;
};

std::string decrire(const Condition& c);