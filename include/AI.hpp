#pragma once

#include <memory>

#include "Fighter.hpp"

enum class Niveau { Faible, Normal, Extreme };

std::unique_ptr<Fighter> creerOrdinateur();
void equiperOrdinateur(Fighter& f, Niveau niveau);
void jouerTourIA(Fighter& ia, Fighter& joueur, Niveau niveau);