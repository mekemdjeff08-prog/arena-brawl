#pragma once
#include <string>

struct Competence {
    std::string nom;
    int coutMana;
    bool offensive;     // true si la competence inflige des degats
    int coutPv;
    int soin;
    int chanceDouble;
    std::string rarete; // Mineure, Speciale, Epique
};

// ===== Regles de degats =====
const int COEF_ATTAQUE_NORMALE = 100;   // en % de la puissance

int coefficientRang(const std::string& rarete);
int coefficientCompetence(const Competence& c);
int calculerDegats(int puissance, int coefPct);