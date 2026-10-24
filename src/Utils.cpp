#include "Utils.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <random>

int nombreAleatoire(int min, int max) {
    static std::mt19937 generateur(std::random_device{}());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generateur);
}

int lireEntier(const std::string& message, int min, int max) {
    while (true) {
        std::cout << message;
        int valeur;
        if (std::cin >> valeur) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (valeur >= min && valeur <= max) return valeur;
        } else {
            if (std::cin.eof()) std::exit(0);
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cout << "Entree invalide : tape un nombre entre " << min
                  << " et " << max << "." << std::endl;
    }
}

std::string lireNom(const std::string& message) {
    std::string nom;
    while (true) {
        std::cout << message;
        if (!std::getline(std::cin, nom)) std::exit(0);
        if (!nom.empty()) return nom;
        std::cout << "Le nom ne peut pas etre vide." << std::endl;
    }
}