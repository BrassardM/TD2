#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include<memory>
#include <iostream>
#include <functional>
#include <span>

using namespace std;
struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class ListeFilms {
private:
	int capacite_, nElements_;
	Film** elements_;
public:
	ListeFilms(const std::string& filename);
	ListeFilms();
	int getnElements() const { return nElements_; }
	Film** getElements() const { return elements_; }

	void ajouterFilms(Film* filmAjout);
	shared_ptr<Acteur> trouverActeur(const std::string& nom) const;
	void enleverFilm(Film* filmEnleve);

	void afficherListeFilms() const;
	//void afficherFilmographieActeur(const std::string& nomActeur) const;

	void changerActeurDateNaissance(const std::string& nomActeur, int Date);

	void deleteComplet();
	void deleteElements();

	Film* getFilmParCritere(const function<bool(Film*)>& critere);

	Film* operator[](int index) {
		return getElements()[index];
	}

	~ListeFilms() {
		deleteElements();
	}
};

template <typename T>
class Liste {
public:
	int capacite;
	int nElements;
	unique_ptr<shared_ptr<T>[]> elements;

	Liste() {
		capacite = 1;
		nElements = 0;
		elements = make_unique<shared_ptr<T>[]>(capacite); // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
	}
	Liste(int capaciteset, int nElementsSet) {
		capacite = capaciteset;
		nElements = nElementsSet;
		elements = make_unique<shared_ptr<T>[]>(capacite); // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
		for (int i = 0; i < capacite; i++) {
			elements[i] = make_shared<T>(T{});
		}
	}
	Liste(const Liste& toCopy) {
		capacite = toCopy.capacite;
		nElements = toCopy.nElements;
		elements = make_unique<shared_ptr<T>[]>(capacite); // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
		for (unsigned i = 0; i < capacite; i++) {
			elements.get()[i] = toCopy.elements.get()[i];
		}
	}

	T& operator[](int index) {
		return *(elements[index]);
	}
};
using ListeActeurs = Liste<Acteur>;

struct Film
{
	std::string titre = "", realisateur = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie = 0, recette = 0; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;

	Film() {
		titre = "";
		realisateur = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
		anneeSortie = 0;
		recette = 0; // Année de sortie et recette globale du film en millions de dollars
		acteurs;
	}

	struct Film(const Film& toCopy) : titre(toCopy.titre), realisateur(toCopy.realisateur), anneeSortie(toCopy.anneeSortie), recette(toCopy.recette), acteurs(toCopy.acteurs) {}
};

struct Acteur
{
	std::string nom = ""; int anneeNaissance = 0; char sexe = ' ';
	//ListeFilms joueDans;
};