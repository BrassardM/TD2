//#pragma region "Includes"//{
//#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.
//
//#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.
//#include "td2.cpp"
//
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <limits>
//#include <algorithm>
//#include <span>
//
//#include "cppitertools/range.hpp"
//
//#include "bibliotheque_cours.hpp"
//#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
//#include "debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).
//
//using namespace std;
//using namespace iter;
//
//#pragma endregion//}
//class ListeFilmsClass {
//private:
//	ListeFilms listefilms_;
//public:
//	ListeFilmsClass(const std::string& filename);
//	void afficherFilms();
//
//
//	~ListeFilmsClass();
//};
//
//ListeFilmsClass::ListeFilmsClass(const std::string& filename = "films.bin") {
//	listefilms_ = creerListe(filename);
//}
//
//void ListeFilmsClass::afficherFilms() {
//	afficherListeFilms(listefilms_);
//}
//ListeFilmsClass::~ListeFilmsClass() {
//	
//}
//
//int main() {
//	ListeFilmsClass a = {};
//	a.afficherFilms();
//}