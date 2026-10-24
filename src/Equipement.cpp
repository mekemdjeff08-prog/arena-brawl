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