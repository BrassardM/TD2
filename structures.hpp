#pragma once
// Structures mémoires pour une collection de films.

#include <string>

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
	Acteur* trouverActeur(const std::string& nom) const;
	void enleverFilm(Film* filmEnleve);

	void afficherListeFilms() const;
	void afficherFilmographieActeur(const std::string& nomActeur) const;

	void changerActeurDateNaissance(const std::string& nomActeur, int Date);

	void deleteComplet();
	void deleteElements();

	~ListeFilms() {
		deleteElements();
	}
};


struct ListeActeurs {
	int capacite, nElements;
	Acteur** elements; // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
};

struct Film
{
	std::string titre = "", realisateur = ""; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie = 0, recette = 0; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs = {};
};

struct Acteur
{
	std::string nom = ""; int anneeNaissance = 0; char sexe = ' ';
	ListeFilms joueDans;
};