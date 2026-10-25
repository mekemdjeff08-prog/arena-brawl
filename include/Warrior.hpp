#pragma once

#include <string>
#include <vector>

#include "Fighter.hpp"

class Warrior : public Fighter {
public:
    Warrior(std::string nom) : Fighter(nom, 250, 12, 70) {
        equiperArme(armesDeClasse()[0]);
        equiperEquipementClasse(equipementsDeClasse()[0]);
    }

    std::vector<Equipement> armesDeClasse() const override {
        return { {"Epee longue", 2, 0, 0}, {"Hache de guerre", 4, 0, 0},
                 {"Masse lourde", 1, 20, 0}, {"Lame du Titan", 10, 0, 0, true, {TypeCondition::DegatsSubis, 150}} };
    }
    std::vector<Equipement> equipementsDeClasse() const override {
        return { {"Bouclier de fer", 0, 50, 0}, {"Armure de plates", 1, 45, 0},
                 {"Armure du Colosse", 5, 50, 0, true, {TypeCondition::PvBas, 40}} };
    }
    std::vector<Competence> competences() const override {
        return {
            {"Frappe rapide",     5, true,   0,  0, 0, "Mineure"},
            {"Second souffle",   10, false,  0, 35, 0, "Mineure"},
            {"Coup de bouclier", 15, true,   0,  0, 0, "Speciale"},
            {"Tranche-Monde",    40, true,   0,  0, 0, "Epique"},
            {"Rempart Eternel",  45, true,   0, 50, 0, "Epique"},
            {"Colere du Titan",  50, true,  10,  0, 0, "Epique"}
        };
    }

    void subirDegats(int degats) override {
        Fighter::subirDegats(degats * 7 / 10);
    }
};