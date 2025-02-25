#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include <span>
#include<memory>
#include <functional>

#include "cppitertools/range.hpp"

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).


using namespace std;
using namespace iter;

#pragma endregion//}

typedef uint8_t UInt8;
typedef uint16_t UInt16;

#pragma region "Fonctions de base pour lire le fichier binaire"//{

UInt8 lireUint8(istream& fichier)
{
	UInt8 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
UInt16 lireUint16(istream& fichier)
{
	UInt16 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUint16(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion//}

//TODO: Une fonction pour ajouter un Film à une ListeFilms, le film existant déjà; on veut uniquement ajouter le pointeur vers le film existant.  Cette fonction doit doubler la taille du tableau alloué, avec au minimum un élément, dans le cas où la capacité est insuffisante pour ajouter l'élément.  Il faut alors allouer un nouveau tableau plus grand, copier ce qu'il y avait dans l'ancien, et éliminer l'ancien trop petit.  Cette fonction ne doit copier aucun Film ni Acteur, elle doit copier uniquement des pointeurs.
void ListeFilms::ajouterFilms(Film* filmAjout) {
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
	elements_[nElements_] = filmAjout;
	nElements_ += 1;
}

//TODO: Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film; la fonction prenant en paramètre un pointeur vers le film à enlever.  L'ordre des films dans la liste n'a pas à être conservé.
void ListeFilms::enleverFilm(Film* filmEnleve) {
	for (unsigned i : range(0, nElements_)) {
		if (elements_[i] == filmEnleve) {
			nElements_ -= 1;
			for (unsigned j : range(int(i), nElements_)) {
				elements_[j] = elements_[j + 1];
			}
			elements_[nElements_] = nullptr;
			return;
		}
	}
}
//TODO: Une fonction pour trouver un Acteur par son nom dans une ListeFilms, qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouvé.  Devrait utiliser span.
shared_ptr<Acteur> ListeFilms::trouverActeur(const string& nom) const {
	span<Film*> spanFilms({ elements_, long unsigned(nElements_) });
	for (Film* film : spanFilms) {
		span<shared_ptr<Acteur>> spanActeur = ((film->acteurs).enSpan());
		for (shared_ptr<Acteur> m : spanActeur) {
			if (m->nom == nom) {
				return m;
			}
		}
	}
	return nullptr;
}

//TODO: Compléter les fonctions pour lire le fichier et créer/allouer une ListeFilms.  La ListeFilms devra être passée entre les fonctions, pour vérifier l'existence d'un Acteur avant de l'allouer à nouveau (cherché par nom en utilisant la fonction ci-dessus).

shared_ptr<Acteur> lireActeur(istream& fichier, const ListeFilms& listeFilms)
{
	shared_ptr<Acteur> acteur = make_shared<Acteur>();
	acteur->nom = lireString(fichier);
	acteur->anneeNaissance = lireUint16(fichier);
	acteur->sexe = lireUint8(fichier);

	shared_ptr<Acteur> acteurTrouver = listeFilms.trouverActeur(acteur->nom);
	if (acteurTrouver != nullptr) {
		return acteurTrouver;
	}

	return acteur; //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
}

Film* lireFilm(istream& fichier, const ListeFilms& listeFilms)
{
	Film* film = new Film;
	film->titre = lireString(fichier);
	film->realisateur = lireString(fichier);
	film->anneeSortie = lireUint16(fichier);
	film->recette = lireUint16(fichier);
	int nElements = lireUint8(fichier);
	ListeActeurs listeActeurs(nElements, nElements);
	film->acteurs = listeActeurs;

	for (int i = 0; i < nElements; i++) {
		shared_ptr<Acteur> Acteur = lireActeur(fichier, listeFilms); //TODO: Placer l'acteur au bon endroit dans les acteurs du film.
		film->acteurs.ajouter(Acteur, i);
		//film->acteurs.elements[i]->joueDans.ajouterFilms(film); //TODO: Ajouter le film à la liste des films dans lesquels l'acteur joue.
	}
	return film; //TODO: Retourner le pointeur vers le nouveau film.
}

ListeFilms::ListeFilms(const string& filename) {
	ifstream fichier(filename, ios::binary);
	fichier.exceptions(ios::failbit);

	int nElements = lireUint16(fichier);
	//TODO: Créer une liste de films vide.
	capacite_ = 1;
	nElements_ = 0;
	Film** elementsListeFilms = new Film * [capacite_];
	elements_ = elementsListeFilms;

	for (int i = 0; i < nElements; i++) {
		ajouterFilms(lireFilm(fichier, *this)); //TODO: Ajouter le film à la liste.
	}
}

ListeFilms::ListeFilms() {
	nElements_ = 0;
	capacite_ = 1;
	Film** elementsListeFilms = new Film * [capacite_];
	elementsListeFilms[0] = nullptr;
	elements_ = elementsListeFilms;
}

//TODO: Une fonction pour détruire un film (relâcher toute la mémoire associée à ce film, et les acteurs qui ne jouent plus dans aucun films de la collection).  Noter qu'il faut enleve le film détruit des films dans lesquels jouent les acteurs.  Pour fins de débogage, affichez les noms des acteurs lors de leur destruction.

void detruireFilm(Film* filmDetruit) {
	/*for (unsigned i : range(0, filmDetruit->acteurs.nElements)) {

		if (filmDetruit->acteurs.elements[i]->joueDans.getnElements() > 1) {
			(filmDetruit->acteurs.elements[i]->joueDans).enleverFilm(filmDetruit);
		}
		else {
			delete filmDetruit->acteurs.elements[i];
		}
	}*/
	delete filmDetruit;
}


//TODO: Une fonction pour détruire une ListeFilms et tous les films qu'elle contient.


void ListeFilms::deleteComplet() {
	for (unsigned i : range(0, nElements_)) {
		detruireFilm(elements_[i]);
	}
}

void ListeFilms::deleteElements() {

	delete[] elements_;
}

void afficherActeur(const Acteur& acteur)
{
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}

//TODO: Une fonction pour afficher un film avec tous ces acteurs (en utilisant la fonction afficherActeur ci-dessus).

void ListeFilms::afficherListeFilms() const
{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = "\n\033[35m-------\033[0m\n";
	//TODO: Changer le for pour utiliser un span.
	span<Film*> spanfilm({ elements_,long unsigned(nElements_) });
	for (Film* n : spanfilm) {
		//TODO: Afficher le film.
		cout << ligneDeSeparation << n->titre << endl;
		span<shared_ptr<Acteur>> spanActeur = (n->acteurs).enSpan();
		for (shared_ptr<Acteur> m : spanActeur) {
			afficherActeur(*m);
		}
	}
}

/*void ListeFilms::afficherFilmographieActeur(const string& nomActeur) const
{
	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
	const shared_ptr<Acteur> acteur = trouverActeur(nomActeur);
	if (acteur == nullptr)
		cout << "Aucun acteur de ce nom" << endl;
	else
		acteur->joueDans.afficherListeFilms();
}*/

void ListeFilms::changerActeurDateNaissance(const string& nomActeur, int Date) {
	trouverActeur(nomActeur)->anneeNaissance = Date;
}
// odd
ostream& operator<<(ostream& os, const Film& film) {
	os << "Titre du film: " << film.titre << endl;
	os << "Année de sortie du film: " << film.anneeSortie << endl;
	os << "Recette du film: " << film.recette << endl;
	os << "Réalisateur du film: " << film.realisateur << endl;
	os << "Acteurs du film: " << endl;

	span<shared_ptr<Acteur>> spanActeur = film.acteurs.enSpan();
	for (auto n : spanActeur) {
		os << n.get()->nom << endl;
	}
	return os;
}

Film* ListeFilms::getFilmParCritere(const function<bool(Film*)>& critere) {
	for (unsigned i = 0; i < nElements_; i += 1) {
		if (critere(elements_[i])) {
			return elements_[i];
		}
	}
	return nullptr;
}

int main()
{
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par défaut.

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	//TODO: Chaque TODO dans cette fonction devrait se faire en 1 ou 2 lignes, en appelant les fonctions écrites.

	//TODO: La ligne suivante devrait lire le fichier binaire en allouant la mémoire nécessaire.  Devrait afficher les noms de 20 acteurs sans doublons (par l'affichage pour fins de débogage dans votre fonction lireActeur).
	ListeFilms listeFilms("films.bin");

	cout << ligneDeSeparation << "Le premier film de la liste est:" << endl;
	//TODO: Afficher le premier film de la liste.  Devrait être Alien.
	// Une facon plus simple de faire ca

	cout << *listeFilms[0];

	//TODO: Afficher le premier film de la liste.  Devrait être Alien.
	cout << ligneDeSeparation << "Sklien, Alien et listeFilms[1] modifiee:" << endl;

	Film skylien = *listeFilms[0];
	skylien.titre = "Skylien";

	// FONCTION AJOUTER
	(skylien.acteurs).ajouter(listeFilms[1]->acteurs[0], 0);
	skylien.acteurs[0]->nom = "Daniel Wroughton Craig";

	cout << "  -- SKYLIEN -- " << endl;
	cout << skylien;
	cout << "  -- Alien -- " << endl;
	cout << *listeFilms[0];
	cout << "  -- ListeFilms[1] -- " << endl;
	cout << *listeFilms[1];

	cout << ligneDeSeparation << "Fonction Lambda avec critere : " << endl;
	int recetteAObtenir = 955;
	auto critere = [&](Film* pFilm) -> bool {
		if (pFilm->recette == recetteAObtenir) { return true; }
		else { return false; } };
	cout << *(listeFilms.getFilmParCritere(critere)) << endl;


	cout << ligneDeSeparation << "Les films sont:" << endl;
	//TODO: Afficher la liste des films.  Il devrait y en avoir 7.
	listeFilms.afficherListeFilms();

	//TODO: Modifier l'année de naissance de Benedict Cumberbatch pour être 1976 (elle était 0 dans les données lues du fichier).  Vous ne pouvez pas supposer l'ordre des films et des acteurs dans les listes, il faut y aller par son nom.
	string bCumberbatch = "Benedict Cumberbatch";
	listeFilms.changerActeurDateNaissance(bCumberbatch, 1976);
	//cout << ligneDeSeparation << "Liste des films où Benedict Cumberbatch joue sont:" << endl;
	//TODO: Afficher la liste des films où Benedict Cumberbatch joue.  Il devrait y avoir Le Hobbit et Le jeu de l'imitation.
	//listeFilms.afficherFilmographieActeur(bCumberbatch);

	//TODO: Détruire et enlever le premier film de la liste (Alien).  Ceci devrait "automatiquement" (par ce que font vos fonctions) détruire les acteurs Tom Skerritt et John Hurt, mais pas Sigourney Weaver puisqu'elle joue aussi dans Avatar.
	detruireFilm(listeFilms.getElements()[0]);
	listeFilms.enleverFilm(listeFilms.getElements()[0]);
	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	//TODO: Afficher la liste des films.
	listeFilms.afficherListeFilms();
	//listeFilms.afficherFilmographieActeur("Acteur qui n'existe pas");
	//TODO: Faire les appels qui manquent pour avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.
	//listeFilms.afficherFilmographieActeur("Cet acteur n'existe pas");

	// LISTE TEXTE
	Liste<string> listeTextes(4, 2);
	*(listeTextes[0]) = "liste texte entree 0";
	*(listeTextes[1]) = "liste texte entree 1";
	Liste<string> listeTextes2 = listeTextes;
	shared_ptr<string> pString = make_shared<string>("ajout d'un pointeur shared dans la liste texte 2");

	// Ajouter FONCTION
	listeTextes2.ajouter(pString, 0);

	*listeTextes[1] = "modification de l'entree 1 de la liste de texte 2 (devrait affecter la liste 1 aussi)";

	cout << endl << "LISTETEXTES[0] : " << *listeTextes[0] << endl;
	cout << "LISTETEXTES[1] : " << *listeTextes[1] << endl;

	cout << "LISTETEXTES2[0] : " << *listeTextes2[0] << endl;
	cout << "LISTETEXTES2[1] : " << *listeTextes2[1] << endl;


	//TODO: Détruire tout avant de terminer le programme.  La bibliothèque de verification_allocation devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	listeFilms.deleteComplet();
}