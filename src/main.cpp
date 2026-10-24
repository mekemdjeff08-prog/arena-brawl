
#include <iostream>
#include <string>
#include <random>
#include <memory>
#include <vector>
#include <limits>
#include <cstdlib>
#include "Utils.hpp"
#include "Equipement.hpp"
#include "Competence.hpp"
#include "Fighter.hpp"
#include "Warrior.hpp"
#include "Mage.hpp"
#include "Archer.hpp"
#include "AI.hpp"


Equipement choisir(const std::string& titre, const std::vector<Equipement>& liste,
                   bool epiqueAutorise) {
    std::cout << "\n" << titre << " :" << std::endl;
    for (int i = 0; i < (int)liste.size(); i++) {
        std::cout << i + 1 << ". " << liste[i].nom;
        if (liste[i].epique)
            std::cout << (epiqueAutorise ? " [EPIQUE]" : " [EPIQUE - verrouille]");
        std::cout << " (+" << liste[i].bonusPuissance << " puissance, +"
                  << liste[i].bonusPv << " PV, +" << liste[i].bonusMana << " mana)"
                  << std::endl;
        if (liste[i].epique)
            std::cout << "     Condition de deblocage en combat : "
                      << decrire(liste[i].condition) << std::endl;
    }
    while (true) {
        int choix = lireEntier("Ton choix : ", 1, (int)liste.size());
        if (liste[choix - 1].epique && !epiqueAutorise) {
            std::cout << "Un seul objet epique au depart : celui-ci se debloquera en combat."
                      << std::endl;
        } else {
            return liste[choix - 1];
        }
    }
}

bool menuCompetences(Fighter& actif, Fighter& cible) {
    std::vector<Competence> liste = actif.competences();
    std::cout << "\nCompetences (mana " << actif.getMana() << "/"
              << actif.getManaMax() << ") :" << std::endl;
    for (int i = 0; i < (int)liste.size(); i++) {
                std::cout << i + 1 << ". [" << liste[i].rarete << "] " << liste[i].nom
                  << " - " << liste[i].coutMana << " mana"
                  << (actif.peutLancer(liste[i]) ? "" : "  (indisponible)")
                  << ((actif.boostEstActif() && liste[i].rarete == "Epique")
                          ? "  <- CRITIQUE + ETOURDISSEMENT" : "")
                  <<(liste[i].soin > 0
                          ? " [soins restants : " + std::to_string(actif.SoinsRestants()) + "]"
                        : std::string (""))
                  << std::endl;
    }
    std::cout << "0. Retour" << std::endl;
    int choix = lireEntier("Ton choix : ", 0, (int)liste.size());
    if (choix == 0) return false;
    if (!actif.peutLancer(liste[choix - 1])) {
        std::cout << "Impossible : pas assez de mana ou de PV !" << std::endl;
        return false;
    }
    actif.lancer(liste[choix - 1], cible);
    return true;
}

void preparer(Fighter& f) {
    std::cout << "\n=== Equipement de " << f.getNom() << " ===" << std::endl;
    bool epiquePris = false;

    Equipement arme = choisir("Choisis ton arme", f.armesDeClasse(), !epiquePris);
    if (arme.epique) epiquePris = true;
    f.equiperArme(arme);

    Equipement equip = choisir("Choisis ton equipement", f.equipementsDeClasse(), !epiquePris);
    if (equip.epique) epiquePris = true;
    f.equiperEquipementClasse(equip);

    f.equiperAccessoire(choisir("Choisis ton accessoire", accessoiresCommuns(), true));

    f.preparerVerrouilles();
    std::cout << std::endl;
    f.afficherStats();
    f.afficherVerrouilles();
}

void jouerTour(Fighter& actif, Fighter& cible) {
        if (actif.estEtourdi()) {
        std::cout << "\n" << actif.getNom() << " est etourdi et passe son tour !" << std::endl;
        actif.finEtourdissement();
        return;
    }
    actif.regenererMana();
    actif.verifierDeblocages();
    cible.verifierDeblocages();
    bool tourFini = false;
    while (!tourFini) {
        std::cout << "\n" << actif.getNom() << " (PV " << actif.getPv() << "/"
                  << actif.getPvMax() << ", Mana " << actif.getMana() << "/"
                  << actif.getManaMax() << ")"
                  << (actif.boostEstActif() ? "  [PUISSANCE x1.5]" : "") << std::endl;
        std::cout << "1. Attaquer  2. Competence  3. Potion de soin ("
                  << actif.getPotions() << ")\n4. Potion de mana ("
                  << actif.getPotionsMana() << ")  5. Potion de puissance ("
                  << actif.getPotionsPuissance() << ")" << std::endl;
        int choix = lireEntier("Ton choix : ", 1, 5);

        if (choix == 2) {
            tourFini = menuCompetences(actif, cible);
        } else if (choix == 3) {
            tourFini = actif.utiliserPotion();
        } else if (choix == 4) {
            tourFini = actif.utiliserPotionMana();
        } else if (choix == 5) {
            actif.utiliserPotionPuissance(); 
        } else {
            actif.attaquer(cible);
            tourFini = true;
        }
    }
    actif.finDeTour();
    std::cout << cible.getNom() << " : " << cible.getPv() << "/"
              << cible.getPvMax() << " PV" << std::endl;
}

std::unique_ptr<Fighter> creerCombattant(int numero) {
    std::cout << "\n--- Joueur " << numero << " ---" << std::endl;
    std::string nom = lireNom("Nom : ");

    std::cout << "1. Guerrier  2. Mage  3. Archer" << std::endl;
    int type = lireEntier("Classe : ", 1, 3);

    if (type == 2) return std::make_unique<Mage>(nom);
    if (type == 3) return std::make_unique<Archer>(nom);
    return std::make_unique<Warrior>(nom);
}




int menuPrincipal() {
    std::cout << "\nARENA BRAWL\n1. Mode 1 VS 1\n2. Mode 1 VS Ordinateur" << std::endl;
    return lireEntier("Votre choix : ", 1, 2);
}

void pause() {
    std::cout << "\n(Appuie sur Entree pour continuer)";
    std::string ligne;
    if (!std::getline(std::cin, ligne)) std::exit(0);
}

Niveau menuNiveau() {
    std::cout << "\nNiveau de l'ordinateur :\n1. Faible\n2. Normal\n3. Extreme" << std::endl;
    int choix = lireEntier("Ton choix : ", 1, 3);
    if (choix == 1) return Niveau::Faible;
    if (choix == 3) return Niveau::Extreme;
    return Niveau::Normal;
}

void jouerPartie(Fighter& a, Fighter& b, bool bEstOrdinateur, Niveau niveau) {
    while (a.estVivant() && b.estVivant()) {
        jouerTour(a, b);
        if (!b.estVivant()) break;
        if (bEstOrdinateur) {
            jouerTourIA(b, a, niveau);
            pause();
        } else {
            jouerTour(b, a);
        }
    }
    std::cout << "\n*** " << (a.estVivant() ? a.getNom() : b.getNom())
              << " remporte le combat ! ***" << std::endl;
}

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