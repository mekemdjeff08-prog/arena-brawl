#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "Utils.hpp"
#include "Equipement.hpp"
#include "Competence.hpp"

class Fighter {
protected:
    std::string nom;
    int pv;
    int pvBase;
    int puissanceBase;
    int mana;
    int manaBase;
    int potions;
    int potionsMana;
    int potionsPuissance;
    bool boostActif;
    int degatsSubis = 0;
    int degatsInfliges = 0;
    int epiquesLancees = 0;
    int coupsDoubles = 0;

    std::vector<Verrouille> verrouilles;
    bool etourdi = false;
    int chanceCritique = 10;
    int soinsUtilises = 0;
    static const int MAX_SOINS_COMPETENCE = 2;

    Equipement arme;
    Equipement equipementClasse;
    Equipement accessoire;

private:
    void changer(Equipement& emplacement, const Equipement& nouveau) {
        pv -= emplacement.bonusPv;
        mana -= emplacement.bonusMana;
        emplacement = nouveau;
        pv += emplacement.bonusPv;
        mana += emplacement.bonusMana;
        if (pv < 1) pv = 1;
        if (mana < 0) mana = 0;
    }

        void frapper(const Competence& c, Fighter& cible, bool critique) {
            int degats = calculerDegats(getPuissance(), coefficientCompetence(c));
        if (critique) {
            degats = degats * 5 / 4;
            std::cout << "  COUP CRITIQUE !" << std::endl;
        }
        degatsInfliges += degats;
        std::cout << "  -> " << degats << " degats sur " << cible.getNom() << std::endl;
        cible.subirDegats(degats);
    }

    bool conditionRemplie(const Condition& c) const {
        switch (c.type) {
            case TypeCondition::DegatsSubis:        return degatsSubis >= c.seuil;
            case TypeCondition::DegatsInfliges:     return degatsInfliges >= c.seuil;
            case TypeCondition::CompetencesEpiques: return epiquesLancees >= c.seuil;
            case TypeCondition::CoupsDoubles:       return coupsDoubles >= c.seuil;
            case TypeCondition::PvBas:              return pv * 100 <= c.seuil * getPvMax();
    }
        return false;
    }

    void equiperSansSoin(Equipement& emplacement, const Equipement& nouveau) {
            int pvAvant = pv ;
            changer (emplacement, nouveau );
            pv = pvAvant;
            if (pv > getPvMax()) pv = getPvMax();
            if (pv < 1) pv = 1;
        }

public:
        Fighter(std::string nom, int pvBase, int puissanceBase, int manaBase)
        : nom(nom), pv(pvBase), pvBase(pvBase), puissanceBase(puissanceBase),
          mana(manaBase), manaBase(manaBase),
          potions(2), potionsMana(1), potionsPuissance(1), boostActif(false),
          arme{"Aucune", 0, 0, 0}, equipementClasse{"Aucun", 0, 0, 0},
          accessoire{"Aucun", 0, 0, 0} {}
    virtual ~Fighter() = default;

    std::string getNom() const { return nom; }
    int getPv() const { return pv; }
    int getMana() const { return mana; }
    int getPvMax() const {
        return pvBase + arme.bonusPv + equipementClasse.bonusPv + accessoire.bonusPv;
    }
    int getManaMax() const {
        return manaBase + arme.bonusMana + equipementClasse.bonusMana + accessoire.bonusMana;
    }
      int getPuissance() const {
        int p = puissanceBase + arme.bonusPuissance
              + equipementClasse.bonusPuissance + accessoire.bonusPuissance;
        if (boostActif) p = p * 5 / 4;
        return p;
    }
    bool estVivant() const { return pv > 0; }

    void equiperArme(const Equipement& e) { changer(arme, e); }
    void equiperEquipementClasse(const Equipement& e) { changer(equipementClasse, e); }
    void equiperAccessoire(const Equipement& e) { changer(accessoire, e); }

    void afficherStats() const {
        std::cout << nom << " | PV " << pv << "/" << getPvMax()
                  << " | Mana " << mana << "/" << getManaMax()
                  << " | Puissance " << getPuissance() << "\n  Arme : " << arme.nom
                  << " | Equip. : " << equipementClasse.nom
                  << " | Accessoire : " << accessoire.nom                  
                  << " | Potions soin/mana/puissance : " << potions << "/"
                  << potionsMana << "/" << potionsPuissance << std::endl;
    }

    virtual std::vector<Equipement> armesDeClasse() const = 0;
    virtual std::vector<Equipement> equipementsDeClasse() const = 0;
    virtual std::vector<Competence> competences() const = 0;

    virtual void subirDegats(int degats) {
        degatsSubis += degats;
        pv -= degats;
        if (pv < 0) pv = 0;
    }

       void soigner(int quantite) {
        int avant = pv;
        pv += quantite;
        if (pv > getPvMax()) pv = getPvMax();
        std::cout << "  -> " << nom << " recupere " << (pv - avant) << " PV" << std::endl;
    }

    int getPotions() const { return potions; }

    bool utiliserPotion() {
        if (potions <= 0) {
            std::cout << "Plus de potion !" << std::endl;
            return false;
        }
        if (pv >= getPvMax()) {
            std::cout << "PV deja au maximum !" << std::endl;
            return false;
        }
        potions--;
        std::cout << nom << " boit une potion (" << potions << " restante(s))" << std::endl;
        soigner(getPvMax() * 30 / 100);
        return true;
    }

    int getPotionsMana() const { return potionsMana; }
    int getPotionsPuissance() const { return potionsPuissance; }
    bool boostEstActif() const { return boostActif; }

    bool utiliserPotionMana() {
        if (potionsMana <= 0) {
            std::cout << "Plus de potion de mana !" << std::endl;
            return false;
        }
        if (mana >= getManaMax()) {
            std::cout << "Mana deja au maximum !" << std::endl;
            return false;
        }
        potionsMana--;
        int avant = mana;
        mana += getManaMax() * 40 / 100;
        if (mana > getManaMax()) mana = getManaMax();
        std::cout << nom << " boit une potion de mana : +" << (mana - avant)
                  << " mana (" << potionsMana << " restante(s))" << std::endl;
        return true;
    }

    bool utiliserPotionPuissance() {
        int prix = getPvMax() * 15 / 100;
        if (potionsPuissance <= 0) {
            std::cout << "Plus de potion de puissance !" << std::endl;
            return false;
        }
        if (boostActif) {
            std::cout << "Le boost est deja actif !" << std::endl;
            return false;
        }
        if (pv <= prix) {
            std::cout << "Pas assez de PV pour supporter la potion (" << prix
                      << " PV necessaires) !" << std::endl;
            return false;
        }
        potionsPuissance--;
        pv -= prix;
        boostActif = true;
        std::cout << nom << " boit une potion de puissance : -" << prix
                  << " PV, puissance x1.5 ce tour !" << std::endl;
        return true;
    }

    void finDeTour() { boostActif = false; }

        void preparerVerrouilles() {
        verrouilles.clear();
        for (const Equipement& e : armesDeClasse())
            if (e.epique && e.nom != arme.nom) verrouilles.push_back({e, true});
        for (const Equipement& e : equipementsDeClasse())
            if (e.epique && e.nom != equipementClasse.nom) verrouilles.push_back({e, false});
    }

    void afficherVerrouilles() const {
        for (const Verrouille& v : verrouilles) {
            std::cout << "  [Verrouille] " << v.objet.nom
                      << " - condition : " << decrire(v.objet.condition) << std::endl;
        }
    }

    void verifierDeblocages() {
        for (int i = 0; i < (int)verrouilles.size(); i++) {
            if (conditionRemplie(verrouilles[i].objet.condition)) {
                std::cout << "\n*** " << nom << " obtient " << verrouilles[i].objet.nom
                          << " ! ***" << std::endl;
                if (verrouilles[i].estArme) equiperSansSoin(arme, verrouilles[i].objet);
                else equiperSansSoin(equipementClasse, verrouilles[i].objet);
            }
        }
    }
    bool estEtourdi() const { return etourdi; }
    void finEtourdissement() { etourdi = false; }

    void etourdir(Fighter& agresseur) {
        if (etourdi) return;
        etourdi = true;
        std::cout << "  *** " << nom << " est ETOURDI et perdra son prochain tour ! ***"
                  << std::endl;
        laisserTomberPotion(agresseur);
    }

    void laisserTomberPotion(Fighter& receveur) {
        if (nombreAleatoire(1, 100) > 40) return; // 40% de chance seulement

        std::vector<int> possibles;
        if (potions > 0) possibles.push_back(0);
        if (potionsMana > 0) possibles.push_back(1);
        if (potionsPuissance > 0) possibles.push_back(2);
        if (possibles.empty()) return;

        int type = possibles[nombreAleatoire(0, (int)possibles.size() - 1)];
        std::string nomPotion;
        if (type == 0) {
            potions--; receveur.potions++;
            nomPotion = "potion de soin";
        } else if (type == 1) {
            potionsMana--; receveur.potionsMana++;
            nomPotion = "potion de mana";
        } else {
            potionsPuissance--; receveur.potionsPuissance++;
            nomPotion = "potion de puissance";
        }
        std::cout << "  " << nom << " laisse tomber une " << nomPotion << " ! "
                  << receveur.nom << " la recupere automatiquement." << std::endl;
    }

    void regenererMana() {
        mana += getManaMax() / 10;
        if (mana > getManaMax()) mana = getManaMax();
    }

    void attaquer(Fighter& cible) {
        int degats = calculerDegats(getPuissance(), COEF_ATTAQUE_NORMALE);
        if (nombreAleatoire(1, 100) <= chanceCritique) {
            degats = degats * 5 / 4;
            std::cout << "COUP CRITIQUE ! ";
        }
        degatsInfliges += degats;
        std::cout << nom << " attaque " << cible.getNom()
                  << " : " << degats << " degats !" << std::endl;
        cible.subirDegats(degats);
    }
    
    bool peutLancer(const Competence& c) const {
        if (c.soin > 0 && soinsUtilises >= MAX_SOINS_COMPETENCE) return false;
        return mana >= c.coutMana && pv > c.coutPv;
    }

    int SoinsRestants() const { return MAX_SOINS_COMPETENCE - soinsUtilises;
    }

    void lancer(const Competence& c, Fighter& cible) {
        mana -= c.coutMana;
        if (c.rarete == "Epique") epiquesLancees++;
        std::cout << nom << " utilise " << c.nom << " !" << std::endl;

        bool combo = (c.rarete == "Epique" && boostActif && c.degatsMaxPct > 0);
        bool critique = combo || nombreAleatoire(1, 100) <= chanceCritique;

        if (c.degatsMaxPct > 0) frapper(c, cible, critique);
        if (c.chanceDouble > 0 && cible.estVivant()
            && nombreAleatoire(1, 100) <= c.chanceDouble) {
            std::cout << "  Coup double !" << std::endl;
            coupsDoubles++;
            frapper(c, cible, critique);
        }
        if (combo && cible.estVivant()) cible.etourdir(*this);
        if (c.soin > 0) {
            soigner(c.soin);
            soinsUtilises++;
        }
        if (c.coutPv > 0) subirDegats(c.coutPv);
    }
};