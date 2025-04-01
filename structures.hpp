#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include <memory>
#include <iostream>
#include <functional>
#include <span>

using namespace std;
class Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class Affichable {
public:
	virtual void afficher(ostream& os) const {}
	virtual ~Affichable() {};
};

class Item : public Affichable {
public:
	string titre;
	int anneeSortie;
	void afficher(ostream& os) const override {
		os << titre;// << endl << " Annee Sortie : " << anneeSortie << endl;
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

template<typename T>
class Node {
public:
	Node(const shared_ptr<T> element) : element_(element) {};
private:
	inline static constexpr Node* past_end = nullptr;
	shared_ptr<T> element_;
	Node* previous_ = past_end;
	Node* next_ = past_end;
	template <typename T> friend class Iterator;
	template <typename T> friend class Liste;
};

template<typename T>
class Iterator {
public:
	Iterator(Node<T>* position = Node<T>::past_end):position_(position) {};
	shared_ptr<T> operator*();
	Iterator operator++();
	Iterator operator--();
	bool operator==(const Iterator& autre) const = default;
private:
	Node<T>* position_;
	template<typename T> friend class Liste;
};
template <typename T>
class Liste {
private:
	int capacite;
	int nElements;
	unique_ptr<shared_ptr<T>[]> elements;
	Node<T>* first_ = Node<T>::past_end;
	Node<T>* last_ = Node<T>::past_end;

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
	Iterator<T> begin() { return Iterator(first_); }
};

using ListeActeurs = Liste<Acteur>;
struct Acteur
{
	std::string nom = ""; int anneeNaissance = 0; char sexe = ' ';
	//ListeFilms joueDans;
};

class Film : virtual public Item {
public:
	string realisateur = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int recette = 0; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
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
	void ajouterEtModifierPremierActeur(Film* film, int index, string nouveauNom) {
		acteurs.ajouter(film->acteurs[0], index);
		acteurs[index]->nom = nouveauNom;
	}
	span<shared_ptr<Acteur>> spanActeur() {
		return acteurs.enSpan();
	}
	void afficher(ostream& os) const override {
		Item::afficher(os);
		os << ", par " << realisateur << endl; //<< " Recette : " << recette << "M$" << endl << " Liste des Acteurs : " << endl;
		/*
		span<shared_ptr<Acteur>> spanActeur = acteurs.enSpan();
		int i{};
		for (auto n : spanActeur) {
			os << "Acteur #" << ++i << "." << endl << "\tNom : " << n.get()->nom << endl << "\tAnnee de naissance : " << n.get()->anneeNaissance << endl << "\tSexe : " << n.get()->sexe << endl;
		}
		os << endl;*/
	}
};
// Check si c'est bien ça qu'on veut dire 
class Livre : virtual public Item {
public:
	string auteur;
	int copies;
	int nombrePages;
	Livre() : copies(0), nombrePages(0) {};
	Livre(string ti, int annee, string aut, int cop, int npag) : Item(ti, annee), auteur(aut), copies(cop), nombrePages(npag) {
	}
	Livre(const Livre& autre) : Item(static_cast<Item>(autre)), auteur(autre.auteur), copies(autre.copies), nombrePages(autre.nombrePages) {}
	void afficher(ostream& os) const override {
		Item::afficher(os);
		os << ", de " << auteur << endl; //<< " Copies vendues : " << copies << "M" << endl << " Nombre de pages : " << nombrePages << endl << endl;
	}
};


class FilmLivre : public Film, public Livre {
public:
	void afficher(ostream& os) const override {
		Item::afficher(os);
		os << ", par " << this->realisateur << ", de " << this->auteur;
	}
	FilmLivre(const Film& film, const Livre& livre) : Item(static_cast<Item>(film)), Film(film), Livre(livre) {}
};