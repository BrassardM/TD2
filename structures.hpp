#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include<memory>
#include <iostream>
#include <functional>
#include <span>
#include <cctype>

using namespace std;
class Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class Affichable {
public:
	virtual void afficher(ostream& os) const {}
	virtual ~Affichable() {};
};

class Item : public Affichable {
private:
	int anneeSortie;
public:
	string titre;
	void afficher(ostream& os) const override {
		os << titre;
	}
	Item() : anneeSortie(0) {}
	Item(string ti, int an) : titre(ti), anneeSortie(an) {}
	Item(const Item& autre) : titre(autre.titre), anneeSortie(autre.anneeSortie) {}

	void modifierTitre(string nTitre) {
		titre = nTitre;
	}
	bool checkTitre(string titreRecherche) {
		return (titre.find(titreRecherche) != std::string::npos);
	};
	bool operator> (const Item& other) const {
		int i{};
		if (other.titre == titre) {
			return false;
		}
		while (true)
		{
			if (i >= other.titre.size()) {
				return false;
			}
			else if (i >= titre.size()) {
				return true;
			}
			else if (toupper(titre[i]) < toupper(other.titre[i]))
				return true;
			else if(toupper(titre[i]) > toupper(other.titre[i]))
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
	int capacite;
	int nElements;
	unique_ptr<shared_ptr<T>[]> elements;
protected:
	T getElements(int i) {
		return *(elements)[i];
	}
public:
	class iterator;
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
	iterator begin() {
		return iterator(0, *this);
	}
	iterator end() {
		return iterator(nElements,*this);
	}
};

using ListeActeurs = Liste<Acteur>;

template <typename T>
class Liste<T>::iterator {
private:
	int pos;
	Liste &obj;
public:
	iterator(int position, Liste& listeInst) : pos(position), obj(listeInst){
	}
	iterator operator++(int){
		pos += 1;
		return *this;
	}
	iterator &operator++(){
		pos += 1;
		return *this;
	}
	T operator*() const {
		return obj.getElements(pos);
	}
	bool operator!=(const iterator &other) const {
		return pos != other.pos;
	}
};

struct Acteur
{
	std::string nom = ""; int anneeNaissance = 0; char sexe = ' ';
	//ListeFilms joueDans;
};
class Film : virtual public Item {
private:
	string realisateur = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
public:
	ListeActeurs acteurs;
	int recette = 0; // Année de sortie et recette globale du film en millions de dollars

	Film() {
		realisateur = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
		recette = 0; // Année de sortie et recette globale du film en millions de dollars
		acteurs;
	}
	Film(string ti, string real, int anne, int rec, ListeActeurs lA) : Item(ti, anne), realisateur(real), recette(rec) {
		acteurs = lA;
	}
	bool compareRecette(int recetteO) {
		return recette == recetteO;
	}
	friend shared_ptr<Acteur> ListeFilms::trouverActeur(const std::string& nom) const;
	friend Film* lireFilm(istream& fichier, const ListeFilms& listeFilms);
	//struct Film(const Film& toCopy) : Item(static_cast<Item>(toCopy)), realisateur(toCopy.realisateur), recette(toCopy.recette), acteurs(toCopy.acteurs) {}
	void ajouterEtModifierPremierActeur(Film* film, int index, string nouveauNom) {
		acteurs.ajouter(film->acteurs[0], index);
		acteurs[index]->nom = nouveauNom;
	}
	span<shared_ptr<Acteur>> spanActeur() {
		return acteurs.enSpan();
	}
	void afficherRealisateur(ostream& os) const {
		os << ", par " << realisateur;
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
	string auteur;
	int copies;
	int nombrePages;
public:
	Livre() : copies(0), nombrePages(0) {};
	Livre(string ti, int annee, string aut, int cop, int npag) : Item(ti, annee), auteur(aut), copies(cop), nombrePages(npag) {
	}
	Livre(const Livre& autre) : Item(static_cast<Item>(autre)), auteur(autre.auteur), copies(autre.copies), nombrePages(autre.nombrePages) {}
	void afficherAuteur(ostream& os) const {
		os << ", de " << auteur;
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