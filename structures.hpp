#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include<memory>
#include <iostream>
#include <functional>
#include <span>
#include <cctype>

using namespace std;

/**
* Module qui contient les classes utilisées dans le module principale et qui nomme les méthodes.
*
* \file   td5.cpp
* \author Jiaqi Zhao et Matthew Brassard
* \date   1er avril 2025
* Créé le 7 février 2025
*/

class Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class Affichable {
public:
	virtual void afficher(ostream& os) const {}
	virtual ~Affichable() {};
};

class Item : public Affichable {
private:
	int anneeSortie_;
public:
	string titre_;
	void afficher(ostream& os) const override {
		os << titre_;
	}
	Item() : anneeSortie_(0) {}
	Item(string titre, int anneeSortie) : titre_(titre), anneeSortie_(anneeSortie) {}
	Item(const Item& autre) : titre_(autre.titre_), anneeSortie_(autre.anneeSortie_) {}

	void modifierTitre(string titreModifier) {
		titre_ = titreModifier;
	}
	bool checkTitre(string titreRecherche) {
		return (titre_.find(titreRecherche) != std::string::npos);
	};
	bool operator> (const Item& other) const {
		int i{};
		if (other.titre_ == titre_) {
			return false;
		}
		while (true)
		{
			if (i >= other.titre_.size() || i >= titre_.size()) {
				return false;
			}
			else if (toupper(titre_[i]) < toupper(other.titre_[i]))
				return true;
			else if(toupper(titre_[i]) > toupper(other.titre_[i]))
				return false;
			i++;
		}
	}
};

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
	span<Film*> enSpan() {
		long unsigned nUns = nElements_;
		return span<Film*>(elements_, nUns);
	}
};

template <typename T>
class Liste {
private:
	int capacite_;
	int nElements_;
	unique_ptr<shared_ptr<T>[]> elements_;
protected:
	T getElements(int i) {
		return *(elements_)[i];
	}
public:
	class iterator;
	Liste() {
		capacite_ = 1;
		nElements_ = 0;
		elements_ = make_unique<shared_ptr<T>[]>(capacite_); // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
	}
	Liste(int capaciteSet, int nElementsSet) {
		capacite_ = capaciteSet;
		nElements_ = nElementsSet;
		elements_ = make_unique<shared_ptr<T>[]>(capacite_); // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
		for (int i = 0; i < capacite_; i++) {
			elements_[i] = make_shared<T>(T{});
		}
	}
	void operator=(Liste<T>& toCopy) {
		capacite_ = toCopy.capacite_;
		nElements_ = toCopy.nElements_;
		elements_ = make_unique<shared_ptr<T>[]>(capacite_); // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
		for (unsigned i = 0; i < capacite_; i++) {
			elements_[i] = toCopy.elements_[i];
		}
	}
	Liste(const Liste& toCopy) {
		capacite_ = toCopy.capacite_;
		nElements_ = toCopy.nElements_;
		elements_ = make_unique<shared_ptr<T>[]>(capacite_); // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
		for (unsigned i = 0; i < capacite_; i++) {
			elements_[i] = toCopy.elements_[i];
		}
	}
	span<shared_ptr<T>> enSpan() const {
		long unsigned nUns = nElements_;
		span<shared_ptr<T>> spanActeur({ elements_.get(), nUns });
		return spanActeur;
	}
	shared_ptr<T> operator[](int index) {
		return elements_[index];
	}
	void ajouter(shared_ptr<T> ajout, int index) {
		elements_[index] = ajout;
	}
	iterator begin() {
		return iterator(0, *this);
	}
	iterator end() {
		return iterator(nElements_,*this);
	}
};

using ListeActeurs = Liste<Acteur>;

template <typename T>
class Liste<T>::iterator {
private:
	int position_;
	Liste& elements_;
public:
	iterator(int position, Liste& listeInst) : position_(position), elements_(listeInst){
	}
	iterator operator++(int){
		position_ += 1;
		return *this;
	}
	iterator &operator++(){
		position_ += 1;
		return *this;
	}
	T operator*() const {
		return elements_.getElements(position_);
	}
	bool operator!=(const iterator &other) const {
		return position_ != other.position_;
	}
};

struct Acteur
{
	std::string nom_ = ""; int anneeNaissance_ = 0; char sexe_ = ' ';
	//ListeFilms joueDans;
};
class Film : virtual public Item {
private:
	string realisateur_ = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
public:
	ListeActeurs acteurs_;
	int recette_ = 0; // Année de sortie et recette globale du film en millions de dollars

	Film() {
		realisateur_ = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
		recette_ = 0; // Année de sortie et recette globale du film en millions de dollars
		acteurs_;
	}
	Film(string titre, string realisateur, int anneeSortie, int recette, ListeActeurs listeActeurs) : Item(titre, anneeSortie), 
																									 realisateur_(realisateur), recette_(recette) {
		acteurs_ = listeActeurs;
	}
	bool compareRecette(int recetteAutre) {
		return recette_ == recetteAutre;
	}
	friend shared_ptr<Acteur> ListeFilms::trouverActeur(const std::string& nom) const;
	friend Film* lireFilm(istream& fichier, const ListeFilms& listeFilms);
	//struct Film(const Film& toCopy) : Item(static_cast<Item>(toCopy)), realisateur(toCopy.realisateur), recette(toCopy.recette), acteurs(toCopy.acteurs) {}
	void ajouterEtModifierPremierActeur(Film* film, int index, string nouveauNom) {
		acteurs_.ajouter(film->acteurs_[0], index);
		acteurs_[index]->nom_ = nouveauNom;
	}
	span<shared_ptr<Acteur>> spanActeur() {
		return acteurs_.enSpan();
	}
	void afficherRealisateur(ostream& os) const {
		os << ", par " << realisateur_;
	}
	void afficher(ostream& os) const override {
		Item::afficher(os);
		afficherRealisateur(os);
		os << endl;
		//os << " Realisateur : " << realisateur << endl << " Recette : " << recette << "M$" << endl << " Liste des Acteurs: " << endl;
		//span<shared_ptr<Acteur>> spanActeur = acteurs.enSpan();
		//int i{};
		//for (auto n : spanActeur) {
		//	os << "Acteur #" << ++i << "." << endl << "\tNom: " << n.get()->nom << endl << "\tAnnee de naissance: " << n.get()->anneeNaissance << endl << "\tSexe (big pause): " << n.get()->sexe << endl << endl;
		//}
	}
};

class Livre : virtual public Item {
private:
	string auteur_;
	int copies_;
	int nPages_;
public:
	Livre() : copies_(0), nPages_(0) {};
	Livre(string titre, int anneeSortie, string auteur, int copies, int nPages) : Item(titre, anneeSortie), 
																						 auteur_(auteur), copies_(copies), nPages_(nPages) {
	}
	Livre(const Livre& autre) : Item(static_cast<Item>(autre)), auteur_(autre.auteur_), copies_(autre.copies_), nPages_(autre.nPages_) {}
	void afficherAuteur(ostream& os) const {
		os << ", de " << auteur_;
	};
	void afficher(ostream& os) const override {
		Item::afficher(os);
		afficherAuteur(os);
		os << endl;
		//os << " Auteur: " << auteur << endl << " Copies vendues: " << copies << "M" << endl << " Nombre de pages: " << nombrePages << endl << endl;
	}
};


class FilmLivre : public Film, public Livre {
public:
	void afficher(ostream& os) const override {
		Item::afficher(os);
		afficherRealisateur(os);
		afficherAuteur(os);
		os << endl;
	}
	FilmLivre(const Film& film, const Livre& livre) : Item(static_cast<Item>(film)), Film(film), Livre(livre) {}
};