#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include <span>
#include<memory>
#include <functional>
#include <vector>
#include <forward_list>
#include <set>
#include <unordered_map>
#include <algorithm>

#include "cppitertools/range.hpp"

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).

/**
* Module principal des travaux pratiques. Contient définition des méthode et le main.
*
* \file   exo1.cpp
* \author Jiaqi Zhao et Matthew Brassard
* \date   1er avril 2025
* Créé le 7 février 2025
*/
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
		span<shared_ptr<Acteur>> spanActeur = ((film->acteurs_).enSpan());
		for (shared_ptr<Acteur> m : spanActeur) {
			if (m->nom_ == nom) {
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
	acteur->nom_ = lireString(fichier);
	acteur->anneeNaissance_ = lireUint16(fichier);
	acteur->sexe_ = lireUint8(fichier);

	shared_ptr<Acteur> acteurTrouver = listeFilms.trouverActeur(acteur->nom_);
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
		film->acteurs_.ajouter(Acteur, i);
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
	cout << "  " << acteur.nom_ << ", " << acteur.anneeNaissance_ << " " << acteur.sexe_ << endl;
}

//TODO: Une fonction pour afficher un film avec tous ces acteurs (en utilisant la fonction afficherActeur ci-dessus).

void ListeFilms::afficherListeFilms() const
{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = "\n\033[35m-------\033[0m\n";
	//TODO: Changer le for pour utiliser un span.
	span<Film*> spanfilm({ elements_,long unsigned(nElements_) });
	for (Film* film : spanfilm) {
		//TODO: Afficher le film.
		//cout << ligneDeSeparation << n->titre << endl;
		span<shared_ptr<Acteur>> spanActeur = film->spanActeur();
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

void ListeFilms::changerActeurDateNaissance(const string& nomActeur, int date) {
	trouverActeur(nomActeur)->anneeNaissance_ = date;
}

ostream& operator<<(ostream& os, const Item& item) {
	item.afficher(os);
	return os;
}

template <typename T>
void afficherConteneur(const T& vectorItems) {
	for (auto&& item : vectorItems) {
		cout << "\n\033[32m════════════════════════\033[0m\n" << endl;
		cout << *item;
	}
	cout << "\n\033[35m════════════════════════════════════════\033[0m\n";
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
	vector<unique_ptr<Item>> vectorItems; //Assignation de vector
	int nFilms{};
	for (Film* film : listeFilms.enSpan()) {
		vectorItems.push_back(make_unique<Film>(*film));
		nFilms++;
	}
	ifstream fichier("livres.txt");

	string nom, auteur;
	int anneeSortie, copies, pages;
	int nLivres{};
	while (fichier >> quoted(nom) >> anneeSortie >> quoted(auteur) >> copies >> pages) {
		Livre livre(nom, anneeSortie, auteur, copies, pages);
		vectorItems.push_back(make_unique<Livre>(livre));
		nLivres++;
	}
	//afficherConteneur(vectorItems);

	vector<Item*> hobbitItems;
	for (auto&& item : vectorItems) {
		if (item->checkTitre("Hobbit")) {
			hobbitItems.push_back(item.get());
		}
	}
	FilmLivre hobbit(*(dynamic_cast<Film*>(hobbitItems[0])), *(dynamic_cast<Livre*>(hobbitItems[1])));
	vectorItems.push_back(make_unique<FilmLivre>(hobbit));

	//cout << "INCLUANT LE COMBO FILMLIVRE HOBBIT" << endl << ligneDeSeparation;
	//afficherConteneur(vectorItems);	
	forward_list<shared_ptr<Item>> items;

	//1.1
	cout << "1.1 Copier les pointeurs du vecteur d’items final de la biliothèque dans une forward_list en O(n)" << endl;
	for (int i = nLivres + nFilms; i >= 0 ; i -= 1) {
		if (i == nLivres + nFilms){
			items.push_front(make_shared<FilmLivre>(*(dynamic_cast<FilmLivre*>(&(*vectorItems[i])))));
		}
		else if (i < nLivres + nFilms && i >= nFilms) {
			items.push_front(make_shared<Livre>(*(dynamic_cast<Livre*>(&(*vectorItems[i])))));
		}
		else {
			items.push_front(make_shared<Film>(*(dynamic_cast<Film*>(&(*vectorItems[i])))));
		}
	}
	afficherConteneur(items);
	//1.2 
	cout << "Copier la liste qui est en ordre original à l’envers(le dernier item se retrouve en premier) en O(n)" << endl;
	forward_list <unique_ptr<Item>> itemsR;
	int i{};
	for (auto&& it = items.begin(); it != items.end(); ++it) {
		if (i < nFilms) {
			itemsR.push_front(make_unique<Film>(*(dynamic_cast<Film*>(&(*(*it))))));
		}
		else if (i < nFilms + nLivres) {
			itemsR.push_front(make_unique<Livre>(*(dynamic_cast<Livre*>(&(*(*it))))));
		}
		else {
			itemsR.push_front(make_unique<FilmLivre>(*(dynamic_cast<FilmLivre*>(&(*(*it))))));
		}
		
		i++;
	}
	afficherConteneur(itemsR);
	//1.3
	cout << "Copier la liste qui est en ordre original dans le même ordre qu’elle est, en O(n) dans une autre forward_list" << endl;
	forward_list <unique_ptr<Item>> itemsOriginalOrdre;
	int j{};
	itemsOriginalOrdre.push_front(make_unique<Film>(*(dynamic_cast<Film*>(&(*(*(items.begin())))))));
	auto itC = (itemsOriginalOrdre.begin());
	for (auto&& it = items.begin(); it != items.end(); ++it){
		if (j == 0) {
			j++;
			continue;
		}
		if (j < nFilms) {
			itemsOriginalOrdre.insert_after(itC,(make_unique<Film>(*(dynamic_cast<Film*>(&(*(*it)))))));
		}
		else if (j < nFilms + nLivres) {
			itemsOriginalOrdre.insert_after(itC, (make_unique<Livre>(*(dynamic_cast<Livre*>(&(*(*it)))))));
		}
		else {
			itemsOriginalOrdre.insert_after(itC, make_unique<FilmLivre>(*(dynamic_cast<FilmLivre*>(&(*(*it))))));
		}
		j++;
		++itC;
	}
	afficherConteneur(itemsOriginalOrdre);
	
	//1.4
	cout << "Copier la liste qui est en ordre original à l’envers dans un vector avec les mêmes contraintes que ci-dessus" << endl;
	int k{};
	vector<unique_ptr<Item>> vectorItemsRev(nFilms + nLivres + 1);
	for (auto&& it = items.begin(); it != items.end(); ++it) {
		int vecIndex = nLivres + nFilms - k;
		if (k < nFilms) {
			vectorItemsRev.at(vecIndex) = (make_unique<Film>(*(dynamic_cast<Film*>(&(*(*it))))));
		}
		else if (k < nFilms + nLivres) {
			vectorItemsRev.at(vecIndex) = (make_unique<Livre>(*(dynamic_cast<Livre*>(&(*(*it))))));
		}
		else {
			vectorItemsRev.at(vecIndex) = (make_unique<FilmLivre>(*(dynamic_cast<FilmLivre*>(&(*(*it))))));
		}
		k++;
	}
	afficherConteneur(vectorItemsRev);
	//O(n)
	
	//1.5 
	for (auto&& acteur : (*(dynamic_cast<Film*>(&(*(vectorItems[0]))))).acteurs_) {
		cout << acteur.nom_ << endl;
	}

	//2.1
	auto cmp = [](shared_ptr<Item> a, shared_ptr<Item>b) {return ((*a) > (*b)); };
	set <shared_ptr<Item>, decltype(cmp)> itemSet;
	
	int h{};
	for (auto&& item : vectorItems) {
		if (!(*item).checkTitre("The Hobbit")&& !(*item).checkTitre("Le Hobbit")) {
			if (h < nFilms) {
				itemSet.insert(make_shared<Film>(*(dynamic_cast<Film*>(&(*item)))));
			}
			else if (h < nFilms + nLivres) {
				itemSet.insert(make_shared<Livre>(*(dynamic_cast<Livre*>(&(*item)))));
			}
		}
		h++;
	}
	itemSet.insert(make_shared<Livre>(*(dynamic_cast<FilmLivre*>(&(*vectorItems[vectorItems.size()-1])))));
	afficherConteneur(itemSet);

	//2.2
	unordered_map<string, shared_ptr<Item>> mapItems;

	int l{};
	for (auto&& item : vectorItems) {
		if (l < nFilms) {
			mapItems.insert({(*item).titre_, make_shared<Film>(*(dynamic_cast<Film*>(&(*item)))) });
		}
		else if (l < nFilms + nLivres) {
			mapItems.insert({ (*item).titre_,(make_shared<Livre>(*(dynamic_cast<Livre*>(&(*item))))) });
		}
		else {
			mapItems.insert({ (*item).titre_, make_shared<FilmLivre>(*(dynamic_cast<FilmLivre*>(&(*item)))) });
		}
		l++;
	}
	cout << *(mapItems["The Hobbit"]);
	//3.1
	vector<shared_ptr<Item>> vFilm;
	auto iterCopy = copy_if(items.begin(), items.end(), back_inserter(vFilm), [&](shared_ptr<Item> i) {if (dynamic_cast<Film*>(i.get()) != NULL) 	return true; else return false; });
	afficherConteneur(vFilm);

	//3.2
	int sum{};
	for_each(vFilm.begin(), vFilm.end(), [&](shared_ptr<Item> i) {sum += (*(dynamic_cast<Film*>(&(*i)))).recette_; });
	cout << "Somme des recettes : " << sum << "M$";

	listeFilms.deleteComplet();
}