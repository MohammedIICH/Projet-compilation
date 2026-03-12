#include "automate.h"
#include "etat.h"
#include <iostream>

using namespace std;

Automate::Automate() : resultat(0), accepte(false), erreur(false), lexer(nullptr) {}

Automate::~Automate() {
   while (!pilEtat.empty())    { delete pilEtat.top();    pilEtat.pop(); }
   while (!pilSymbole.empty()) { delete pilSymbole.top(); pilSymbole.pop(); }
}

// Boucle principale LALR(1) avec pattern State.
// Chaque état décide lui-même d'un décalage, d'une réduction ou de l'acceptation.
bool Automate::Analyser(const string& expression) {
   // Réinitialisation
   while (!pilEtat.empty())    { delete pilEtat.top();    pilEtat.pop(); }
   while (!pilSymbole.empty()) { delete pilSymbole.top(); pilSymbole.pop(); }
   accepte = false;
   erreur  = false;
   resultat = 0;

   Lexer lex(expression);
   lexer = &lex;

   pilEtat.push(new Etat0());

   while (!accepte && !erreur) {
      Symbole* sym = lexer->Consulter();
      pilEtat.top()->transition(*this, sym);
   }

   lexer = nullptr;

   // Nettoyage des états et symboles restants
   while (!pilEtat.empty())    { delete pilEtat.top();    pilEtat.pop(); }
   while (!pilSymbole.empty()) { delete pilSymbole.top(); pilSymbole.pop(); }

   return accepte;
}

void Automate::Afficher() {
   cout << "  * Resultat: " << resultat << endl;
}
