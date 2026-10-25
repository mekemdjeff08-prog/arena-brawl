#pragma once

#include <string>
#include <vector>

#include "Fighter.hpp"

class Mage : public Fighter {
public:
    Mage(std::string nom) : Fighter(nom, 180, 8, 250) {
        equiperArme(armesDeClasse()[0]);
        equiperEquipementClasse(equipementsDeClasse()[0]);
    }

    std::vector<Equipement> armesDeClasse() const override {
        return { {"Baton de feu", 3, 0, 0}, {"Baton de sagesse", 2, 15, 20},
                 {"Sceptre des Etoiles", 12, 0, 40, true, {TypeCondition::CompetencesEpiques, 3}} };
    }
    std::vector<Equipement> equipementsDeClasse() const override {
        return { {"Robe enchantee", 0, 20, 30}, {"Grimoire ancien", 3, 0, 20},
                 {"Couronne du Neant", 10, 10, 60, true, {TypeCondition::DegatsInfliges, 200}} };
    }
    std::vector<Competence> competences() const override {
        return {
            {"Etincelle",             10, true,   0,  0,  0, "Mineure"},
            {"Bouclier de givre",     20, false,  0, 40,  0, "Mineure"},
            {"Boule de feu",          45, true,  15,  0,  0, "Speciale"},
            {"Meteore",              100, true,  20,  0,  0, "Epique"},
            {"Foudre celeste",       110, true,   0,  0, 30, "Epique"},
            {"Renaissance arcanique",120, true,   0, 50,  0, "Epique"}
        };
    }
};