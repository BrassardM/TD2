#pragma region "Includes"{
#define _CRT_SECURE_NO_WARNINGS  //On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"       //Structures de données pour la collection de films en mémoire.
#include "td2.cpp"

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include <span>

#include "cppitertools/range.hpp"

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp"  //Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp"         //Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).

using namespace std;
using namespace iter;

#pragma endregion}
class ListeFilmsClass {
public:
	ListeFilmsClass() {
		capacite_ = 1;
		nElements_ = 0;
		elements_ = new Film * [capacite_];

	};
	ListeFilmsClass(string nomFichier);
	void ajouterFilms(Film* filmAAjouter);
	void enleverFilm(Film* filmAEnlever);
	// Voir si un acteur est dans listeFilm ou non
	Acteur* trouverActeur(const string& nom);
private:
	int capacite_;
	int nElements_;
	Film** elements_;
};

void ListeFilmsClass::ajouterFilms(Film* filmAAjouter) {
	if (nElements_ == capacite_) {
		capacite_ *= 2;
		Film** newElements = new Film * [capacite_];
		for (unsigned i : range(0, capacite_)) {
			if (i < nElements_) {
				newElements[i] = elements_[i];
			}
			else {
				newElements[i] = nullptr;
			}
		}
		delete[] elements_;
		elements_ = newElements;
	}
	elements_[nElements_] = filmAAjouter;
	nElements_ += 1;
}

void ListeFilmsClass::enleverFilm(Film* filmAEnlever) {
	for (unsigned i : range(0, nElements_)) {
		if (elements_[i] == filmAEnlever) {
			nElements_ -= 1;
			for (unsigned j : range(int(i), nElements_)) {
				elements_[j] = elements_[j + 1];
			}
			elements_[nElements_] = nullptr;
			return;
		}
	}
}

Acteur* ListeFilmsClass::trouverActeur(const string& nom) {
	for (unsigned i : range(0, nElements_)) {
		for (unsigned j : range(0, elements_[i]->acteurs.nElements)) {
			if (elements_[i]->acteurs.elements[j]->nom == nom) {
				return elements_[i]->acteurs.elements[j];
			}
		}
	}
	return nullptr;
}

ListeFilmsClass::ListeFilmsClass(string nomFichier) {
}


int main() {
	cout << "done";
}