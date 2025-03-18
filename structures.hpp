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
private:
	int capacite;
	int nElements;
	unique_ptr<shared_ptr<T>[]> elements;

public:
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
	void operator=(Liste<T>& toCopy) {
		capacite = toCopy.capacite;
		nElements = toCopy.nElements;
		elements = make_unique<shared_ptr<T>[]>(capacite); // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
		for (unsigned i = 0; i < capacite; i++) {
			elements[i] = toCopy.elements[i];
		}
	}
	Liste(const Liste& toCopy) {
		capacite = toCopy.capacite;
		nElements = toCopy.nElements;
		elements = make_unique<shared_ptr<T>[]>(capacite); // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
		for (unsigned i = 0; i < capacite; i++) {
			elements[i] = toCopy.elements[i];
		}
	}
	span<shared_ptr<T>> enSpan() const {
		long unsigned nUns = nElements;
		span<shared_ptr<T>> spanActeur({ elements.get(), nUns });
		return spanActeur;
	}
	shared_ptr<T> operator[](int index) {
		return elements[index];
	}
	void ajouter(shared_ptr<T> ajout, int index) {
		elements[index] = ajout;
	}
};
using ListeActeurs = Liste<Acteur>;

//  Qu'est ce qu'on devrait mettre dans Affichable?
struct Affichable {
public:

};

struct Item {
	Item(): titre(""), anneeSortie(0) {};
	Item(string titreDonnee, int annee):titre(titreDonnee), anneeSortie(annee) {};
	Item(const Item& autre):titre(autre.titre),anneeSortie(autre.anneeSortie) {};
	string titre;
	int anneeSortie;
};

// On doit changer Film, Livre et Item en classe? Quelle numero dit ca?
struct Film: virtual public Item
{
	std::string realisateur = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int recette = 0; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;

	Film() :realisateur(""), recette(0) {};

	struct Film(const Film& toCopy) : Item(toCopy.titre, toCopy.anneeSortie),realisateur(toCopy.realisateur), recette(toCopy.recette), acteurs(toCopy.acteurs) {}
};
struct Livre: virtual public Item {
	Livre() : auteur(""), millionsCopiesVendues(0), nbPages(0) {};
	Livre(string titre, int annee, string auteurDonnee, int millionsEnVente, int nbPagesDonnees): Item(titre, annee), auteur(auteurDonnee),
		millionsCopiesVendues(millionsEnVente), nbPages(nbPagesDonnees){};
	Livre(const Livre& autre): Item(autre.titre, autre.anneeSortie), auteur(autre.auteur), millionsCopiesVendues(autre.millionsCopiesVendues), nbPages(autre.nbPages) {}
	string auteur; 
	int millionsCopiesVendues, nbPages;
};
// Comment on devrait faire constructeur de LivreFilm?
struct FilmLivre: public Film, public Livre {
	FilmLivre(const Livre& livre, const Film& film):Livre(livre),Film(film) {};
};

struct Acteur
{
	std::string nom = ""; int anneeNaissance = 0; char sexe = ' ';
	//ListeFilms joueDans;
};