#include "AI.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "Utils.hpp"
#include "Equipement.hpp"
#include "Competence.hpp"
#include "Warrior.hpp"
#include "Mage.hpp"
#include "Archer.hpp"

bool chance(int surDix) {
    return nombreAleatoire(1, 10) <= surDix;
}

int valeurCompetence(const Fighter& f, const Competence& c) {
    int p = f.getPuissance();
    int degats = p * coefficientCompetence(c) / 100;
    degats += degats * c.chanceDouble / 100;
    int manque = f.getPvMax() - f.getPv();
    int soin =  (c.soin < manque ? c.soin : manque) / 2;
    return degats + soin - c.coutPv;
}

int degatsMoyens(const Fighter& f, const Competence& c) {
        return f.getPuissance() * coefficientCompetence(c) / 100;
}

int rangRarete(const std::string& rarete) {
    if (rarete == "Mineure") return 0;
    if (rarete == "Speciale") return 1;
    return 2;
}

int meilleureCompetence(const Fighter& f, int rangMax, int reserveMana) {
    std::vector<Competence> liste = f.competences();
    int meilleur = -1;
    int meilleureValeur = 0;
    for (int i = 0; i < (int)liste.size(); i++) {
        const Competence& c = liste[i];
        if (!f.peutLancer(c)) continue;
        if (rangRarete(c.rarete) > rangMax) continue;
        if (c.degatsMaxPct == 0 && f.getPv() * 10 >= f.getPvMax() * 50) continue;
        if (f.getMana() - c.coutMana < reserveMana) continue;
        if (c.coutPv > 0 && f.getPv() - c.coutPv < f.getPvMax() / 4) continue;
        int v = valeurCompetence(f, c);
        if (v <= 0) continue;
        if (meilleur == -1 || v > meilleureValeur) {
            meilleur = i;
            meilleureValeur = v;
        }
    }
    return meilleur;
}

int prixPotionPuissance(const Fighter& f) {
    return f.getPvMax() * 15 / 100;
}


int meilleurCombo(const Fighter& f, int pvMinPct) {
    int prix = prixPotionPuissance(f);
    std::vector<Competence> liste = f.competences();
    int meilleur = -1;
    int meilleureValeur = 0;
    for (int i = 0; i < (int)liste.size(); i++) {
        const Competence& c = liste[i];
        if (c.rarete != "Epique" || c.degatsMaxPct == 0) continue;
        if (!f.peutLancer(c)) continue;
        int pvApres = f.getPv() - prix - c.coutPv;
        if (pvApres <= 0 || pvApres * 100 < f.getPvMax() * pvMinPct) continue;
        int v = valeurCompetence(f, c);
        if (meilleur == -1 || v > meilleureValeur) {
            meilleur = i;
            meilleureValeur = v;
        }
    }
    return meilleur;
}

int coutEpiqueMin(const Fighter& f) {
    int mini = 0;
    for (const Competence& c : f.competences())
        if (c.rarete == "Epique" && c.degatsMaxPct > 0 && (mini == 0 || c.coutMana < mini))
            mini = c.coutMana;
    return mini;
}

bool tenterCombo(Fighter& ia, Fighter& joueur, int pvMinPct) {
    if (ia.boostEstActif() || ia.getPotionsPuissance() <= 0) return false;
    int idx = meilleurCombo(ia, pvMinPct);
    if (idx == -1) return false;
    if (!ia.utiliserPotionPuissance()) return false;
    ia.lancer(ia.competences()[idx], joueur);
    return true;
}

int scoreEquipement(const Equipement& e) {
    return e.bonusPuissance * 4 + e.bonusPv / 2 + e.bonusMana / 3;
}

Equipement meilleurEquipement(const std::vector<Equipement>& liste, bool epique) {
    int best = -1;
    for (int i = 0; i < (int)liste.size(); i++) {
        if (liste[i].epique != epique) continue;
        if (best == -1 || scoreEquipement(liste[i]) > scoreEquipement(liste[best])) best = i;
    }
    return liste[best];
}

Equipement choisirPourIA(const std::vector<Equipement>& liste, bool epique) {
    std::vector<Equipement> candidats;
    for (const Equipement& e : liste)
        if (e.epique == epique) candidats.push_back(e);
    return candidats[nombreAleatoire(0, (int)candidats.size() - 1)];
}

void equiperOrdinateur(Fighter& f, Niveau niveau) {
    if (niveau == Niveau::Extreme) {
        bool epiqueSurArme =
            scoreEquipement(meilleurEquipement(f.armesDeClasse(), true))
            >= scoreEquipement(meilleurEquipement(f.equipementsDeClasse(), true));
        f.equiperArme(meilleurEquipement(f.armesDeClasse(), epiqueSurArme));
        f.equiperEquipementClasse(meilleurEquipement(f.equipementsDeClasse(), !epiqueSurArme));
        f.equiperAccessoire(meilleurEquipement(accessoiresCommuns(), false));
    } else {
        bool epiqueSurArme = nombreAleatoire(0, 1) == 0;
        f.equiperArme(choisirPourIA(f.armesDeClasse(), epiqueSurArme));
        f.equiperEquipementClasse(choisirPourIA(f.equipementsDeClasse(), !epiqueSurArme));
        std::vector<Equipement> acc = accessoiresCommuns();
        f.equiperAccessoire(acc[nombreAleatoire(0, (int)acc.size() - 1)]);
    }
    f.preparerVerrouilles();
}

std::unique_ptr<Fighter> creerOrdinateur() {
    int type = nombreAleatoire(1, 3);
    if (type == 1) return std::make_unique<Warrior>("Ordinateur (Guerrier)");
    if (type == 2) return std::make_unique<Mage>("Ordinateur (Mage)");
    return std::make_unique<Archer>("Ordinateur (Archer)");
}


void tourFaible(Fighter& ia, Fighter& joueur) {
    if (chance(2) && tenterCombo(ia, joueur, 0)) return;

    if (ia.getPotions() > 0 && ia.getPv() * 100 < ia.getPvMax() * 30
        && ia.utiliserPotion()) return;
    if (ia.getPotionsMana() > 0 && ia.getMana() * 100 < ia.getManaMax() * 25
        && ia.utiliserPotionMana()) return;

    int idx = meilleureCompetence(ia, 0, 0);   
    if (idx != -1 && chance(5)) {
        ia.lancer(ia.competences()[idx], joueur);
        return;
    }
    ia.attaquer(joueur);
}


void tourNormal(Fighter& ia, Fighter& joueur) {
    if (chance(7) && tenterCombo(ia, joueur, 20)) return;

    if (ia.getPotions() > 0 && ia.getPv() * 100 < ia.getPvMax() * 35
        && ia.utiliserPotion()) return;
    if (ia.getPotionsMana() > 0 && ia.getMana() * 100 < ia.getManaMax() * 25
        && ia.utiliserPotionMana()) return;

    int rangMax = (nombreAleatoire(1, 100) <= 15) ? 2 : 1;  // epique : rare
    int idx = meilleureCompetence(ia, rangMax, 0);
    if (idx != -1 && valeurCompetence(ia, ia.competences()[idx]) > ia.getPuissance() * COEF_ATTAQUE_NORMALE / 100) {
        ia.lancer(ia.competences()[idx], joueur);
        return;
    }
    ia.attaquer(joueur);
}

void tourExtreme(Fighter& ia, Fighter& joueur) {
    std::vector<Competence> liste = ia.competences();

    int finisher = -1;
    int degatsFinisher = 0;
    for (int i = 0; i < (int)liste.size(); i++) {
        if (!ia.peutLancer(liste[i]) || liste[i].degatsMaxPct == 0) continue;
        int d = degatsMoyens(ia, liste[i]);
        if (d >= joueur.getPv() && (finisher == -1 || d < degatsFinisher)) {
            finisher = i;
            degatsFinisher = d;
        }
    }
    if (finisher != -1) {
        ia.lancer(liste[finisher], joueur);
        return;
    }

  
    if (tenterCombo(ia, joueur, 30)) return;

    int danger = joueur.getPuissance() * 3;
    bool pvBas = ia.getPv() * 100 < ia.getPvMax() * 50 || ia.getPv() <= danger;
    if (pvBas && ia.getPotions() > 0 && ia.getPv() * 100 < ia.getPvMax() * 80
        && ia.utiliserPotion()) return;

    bool comboEnVue = ia.getPotionsPuissance() > 0
                      && ia.getPv() * 100 >= ia.getPvMax() * 45;
    int reserve = comboEnVue ? coutEpiqueMin(ia) : 0;
    bool manaInsuffisant = (comboEnVue && ia.getMana() < reserve)
                           || ia.getMana() * 100 < ia.getManaMax() * 25;
    if (manaInsuffisant && ia.getPotionsMana() > 0 && ia.utiliserPotionMana()) return;

    int idx = meilleureCompetence(ia, comboEnVue ? 1 : 2, reserve);
    if (idx != -1 && valeurCompetence(ia, liste[idx]) > ia.getPuissance() * COEF_ATTAQUE_NORMALE / 100) {
        ia.lancer(liste[idx], joueur);
        return;
    }

    ia.attaquer(joueur);
}

void jouerTourIA(Fighter& ia, Fighter& joueur, Niveau niveau) {
    if (ia.estEtourdi()) {
        std::cout << "\n" << ia.getNom() << " est etourdi et passe son tour !" << std::endl;
        ia.finEtourdissement();
        return;
    }
    ia.regenererMana();
    std::cout << "\n--- Tour de " << ia.getNom() << " (PV " << ia.getPv() << "/"
              << ia.getPvMax() << ", Mana " << ia.getMana() << "/"
              << ia.getManaMax() << ") ---" << std::endl;

    if (niveau == Niveau::Faible) tourFaible(ia, joueur);
    else if (niveau == Niveau::Normal) tourNormal(ia, joueur);
    else tourExtreme(ia, joueur);

    ia.finDeTour();
    ia.verifierDeblocages();
    joueur.verifierDeblocages();
    std::cout << joueur.getNom() << " : " << joueur.getPv() << "/"
              << joueur.getPvMax() << " PV" << std::endl;
}