
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
            {"Frappe rapide",     5,  60,  90,  0,  0, 0, "Mineure"},
            {"Second souffle",   10,   0,   0,  0, 35, 0, "Mineure"},
            {"Coup de bouclier", 15, 100, 140,  0,  0, 0, "Speciale"},
            {"Tranche-Monde",    40, 180, 230,  0,  0, 0, "Epique"},
            {"Rempart Eternel",  45,  90, 120,  0, 50, 0, "Epique"},
            {"Colere du Titan",  50, 220, 280, 10,  0, 0, "Epique"}
        };
    }

    void subirDegats(int degats) override {
        Fighter::subirDegats(degats * 7 / 10);
    }
};

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
            {"Etincelle",             10,  60,  90, 0,  0,  0, "Mineure"},
            {"Bouclier de givre",     20,   0,   0, 0, 40,  0, "Mineure"},
            {"Boule de feu",          45, 140, 190, 15, 0,  0, "Speciale"},
            {"Meteore",              100, 250, 320, 20, 0,  0, "Epique"},
            {"Foudre celeste",       110, 200, 260, 0,  0, 30, "Epique"},
            {"Renaissance arcanique",120, 120, 160, 0, 50,  0, "Epique"}
        };
    }
};

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

std::vector<Equipement> accessoiresCommuns() {
    return { {"Anneau de force", 1, 0, 0}, {"Amulette de vie", 0, 15, 0},
             {"Pendentif de mana", 0, 0, 20}, {"Talisman du chasseur", 1, 5, 5} };
}

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

enum class Niveau { Faible, Normal, Extreme };

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