#include "Equipement.hpp"

std::string decrire(const Condition& c) {
    std::string n = std::to_string(c.seuil);
    switch (c.type) {
        case TypeCondition::DegatsSubis:        return "encaisser " + n + " degats au total";
        case TypeCondition::DegatsInfliges:     return "infliger " + n + " degats au total";
        case TypeCondition::CompetencesEpiques: return "lancer " + n + " competences epiques";
        case TypeCondition::CoupsDoubles:       return "reussir " + n + " coups doubles";
        case TypeCondition::PvBas:              return "tomber a " + n + "% de PV ou moins";
    }
    return "";
}

std::vector<Equipement> accessoiresCommuns() {
    return { {"Anneau de force", 1, 0, 0}, {"Amulette de vie", 0, 15, 0},
             {"Pendentif de mana", 0, 0, 20}, {"Talisman du chasseur", 1, 5, 5} };
}