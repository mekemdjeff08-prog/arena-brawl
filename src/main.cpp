
#include <iostream>
#include <memory>

#include "AI.hpp"
#include "Fighter.hpp"
#include "Game.hpp"

int main() {
    int mode = menuPrincipal();

    if (mode == 1) {
        std::unique_ptr<Fighter> a = creerCombattant(1);
        std::unique_ptr<Fighter> b = creerCombattant(2);
        preparer(*a);
        preparer(*b);
        jouerPartie(*a, *b, false, Niveau::Normal);
    } else {
        Niveau niveau = menuNiveau();
        std::unique_ptr<Fighter> joueur = creerCombattant(1);
        std::unique_ptr<Fighter> ordi = creerOrdinateur();
        preparer(*joueur);

        equiperOrdinateur(*ordi, niveau);
        std::cout << "\n=== Ton adversaire ===" << std::endl;
        ordi->afficherStats();
        ordi->afficherVerrouilles();

        jouerPartie(*joueur, *ordi, true, niveau);
    }
    return 0;
}