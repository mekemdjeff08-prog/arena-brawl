#pragma once

#include <memory>

#include "AI.hpp"
#include "Fighter.hpp"

int menuPrincipal();
Niveau menuNiveau();
std::unique_ptr<Fighter> creerCombattant(int numero);
void preparer(Fighter& f);
void jouerPartie(Fighter& a, Fighter& b, bool bEstOrdinateur, Niveau niveau);