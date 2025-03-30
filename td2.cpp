#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <span>
#include <memory>
#include <functional>
#include <vector>
#include <forward_list>
#include <ranges>


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

// C'est ici les constantes globales?
static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";


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
	string titre = lireString(fichier);
	string realisateur = lireString(fichier);
	int anneeSortie = lireUint16(fichier);
	int recette = lireUint16(fichier);
	int nElements = lireUint8(fichier);
	ListeActeurs listeActeurs(nElements, nElements);
	Film* film = new Film(titre, realisateur, anneeSortie, recette, listeActeurs);

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
		//cout << ligneDeSeparation << n->titre << endl;
		span<shared_ptr<Acteur>> spanActeur = n->spanActeur();
		for (shared_ptr<Acteur> m : spanActeur) {
			afficherActeur(*m);
		}
	}
}


void ListeFilms::changerActeurDateNaissance(const string& nomActeur, int Date) {
	trouverActeur(nomActeur)->anneeNaissance = Date;
}

ostream& operator<<(ostream& os, const Item& item) {
	item.afficher(os);
	return os;
}
// Donc on veut 2 types d'affichages, un type long et un type court et on peut spécifier quel type utiliser?
template<typename T>
void afficherVecteur(const T& vectorItems) {
	int nItem = 0;
	for (auto&& n : vectorItems) {
		cout << nItem << ". " << *n << ligneDeSeparation;
		++nItem;
	}
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

	//TODO: Chaque TODO dans cette fonction devrait se faire en 1 ou 2 lignes, en appelant les fonctions écrites.

	//TODO: La ligne suivante devrait lire le fichier binaire en allouant la mémoire nécessaire.  Devrait afficher les noms de 20 acteurs sans doublons (par l'affichage pour fins de débogage dans votre fonction lireActeur).
	ListeFilms listeFilms("films.bin");
	vector<unique_ptr<Item>> vectorItems; //Assignation de vector
	int nFilms = 0;
	for (Film* n : listeFilms.enSpan()) {
		++nFilms;
		vectorItems.push_back(make_unique<Film>(*n));
	}
	ifstream fichier("livres.txt");

	string nom, auteur;
	int anneeSortie, copies, pages;
	int nLivres{};
	while (fichier >> quoted(nom) >> anneeSortie >> quoted(auteur) >> copies >> pages) {
		nLivres++;
		Livre livre(nom, anneeSortie, auteur, copies, pages);
		vectorItems.push_back(make_unique<Livre>(livre));
	}
	afficherVecteur(vectorItems);
	// 1.1 Original order (1, 2, 3)
	// 1.2 Reverse order (3, 2, 1)
	// 1.3 Original order (1, 2, 3)
	// 1.4 Original order in vector from list (1, 2, 3)
	forward_list<unique_ptr<Item>> listOrdreOriginal;
	for (int i : range(nLivres)) {
		listOrdreOriginal.push_front(make_unique<Livre>(*(dynamic_cast<Livre*>(&(*(vectorItems[vectorItems.size() - i - 1]))))));
	}
	for (int i : range(nFilms)) {
		listOrdreOriginal.push_front(make_unique<Film>(*(dynamic_cast<Film*>(&(*(vectorItems[vectorItems.size() - nLivres - i - 1]))))));
	}
	cout << "Forward list avec même ordre: " << ligneDeSeparation << endl;
	afficherVecteur(listOrdreOriginal);
	

	vector<Item*> hobbitItems;
	for (auto&& n : vectorItems) {
		if (n->checkTitre("Hobbit")) {
			hobbitItems.push_back(n.get());
		}
	}
	FilmLivre hobbit(*(dynamic_cast<Film*>(hobbitItems[0])), *(dynamic_cast<Livre*>(hobbitItems[1])));
	vectorItems.push_back(make_unique<FilmLivre>(hobbit));

	cout << "INCLUANT LE COMBO FILMLIVRE HOBBIT" << endl << ligneDeSeparation;
	afficherVecteur(vectorItems);


	listeFilms.deleteComplet();
}