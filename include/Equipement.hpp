#pragma once
#include <string>
#include <vector>

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
std::vector<Equipement> accessoiresCommuns();