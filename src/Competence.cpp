#include "Competence.hpp"
#include "Utils.hpp"

int coefficientRang(const std::string& rarete) {
    if (rarete == "Mineure") return 130;
    if (rarete == "Speciale") return 200;
    return 300;                          // Epique
}

int coefficientCompetence(const Competence& c) {
    if (c.degatsMaxPct == 0) return 0;               // competence sans degats
    int coef = coefficientRang(c.rarete);
    if (c.soin > 0) coef = coef * 50 / 100;          // effet de soin : -50 %
    if (c.chanceDouble > 0) coef = coef * 70 / 100;  // coup double : -30 %
    return coef;
}

int calculerDegats(int puissance, int coefPct) {
    int base = puissance * coefPct / 100;
    return nombreAleatoire(base * 85 / 100, base * 115 / 100);
}