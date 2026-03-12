#include "symbole.h"
#include <iostream>

using namespace std;

static const string Etiquettes[] = {
   "OPENPAR", "CLOSEPAR", "PLUS", "MULT", "INT", "FIN", "ERREUR", "E", "E_PRIME"
};

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
