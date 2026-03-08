#include "symbole.h"
#include <iostream>

using namespace std;

void Symbole::Affiche() {
   cout << Etiquettes[ident];
}

void Entier::Affiche() {
   Symbole::Affiche();
   cout << "(" << valeur << ")";
}

void Expression::Affiche() {
   Symbole::Affiche();
   cout << "(" << valeur << ")";
}

