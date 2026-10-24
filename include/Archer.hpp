#pragma once

#include <string>
#include <vector>

#include "Fighter.hpp"

class Archer : public Fighter {
public:
      Archer(std::string nom) : Fighter(nom, 200, 10, 120) {
        chanceCritique = 20;
        equiperArme(armesDeClasse()[0]);
        equiperEquipementClasse(equipementsDeClasse()[0]);
    }

    std::vector<Equipement> armesDeClasse() const override {
        return { {"Arc long", 3, 0, 0}, {"Arc court", 2, 10, 0},
                 {"Arc composite", 1, 15, 10}, {"Arc du Faucon", 8, 0, 10, true, {TypeCondition::CoupsDoubles, 2}} };
    }
    std::vector<Equipement> equipementsDeClasse() const override {
        return { {"Carquois magique", 2, 0, 0}, {"Cape de cuir", 0, 20, 10},
                 {"Cape de l'Aigle", 8, 25, 0, true, {TypeCondition::PvBas, 30}} };
    }
    std::vector<Competence> competences() const override {
        return {
            {"Tir precis",          8,  70, 100, 0,  0,  0, "Mineure"},
            {"Fleche empoisonnee", 15,  90, 120, 0,  0,  0, "Mineure"},
            {"Double tir",         25,  70, 100, 0,  0, 50, "Speciale"},
            {"Pluie de fleches",   55, 150, 200, 0,  0, 40, "Epique"},
            {"Vent guerrier",      60, 110, 150, 0, 30,  0, "Epique"},
            {"Tir du faucon",      70, 220, 280, 0,  0,  0, "Epique"}
        };
    }
};