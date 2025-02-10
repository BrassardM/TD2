//#pragma region "Includes"//{
//#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de cha�nes qui sont consid�r�es non s�curitaires.
//
//#include "structures.hpp"      // Structures de donn�es pour la collection de films en m�moire.
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
//#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de m�moire.
//#include "debogage_memoire.hpp"        // Ajout des num�ros de ligne des "new" dans le rapport de fuites.  Doit �tre apr�s les include du syst�me, qui peuvent utiliser des "placement new" (non support� par notre ajout de num�ros de lignes).
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