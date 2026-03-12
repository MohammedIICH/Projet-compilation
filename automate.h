#pragma once

#include <stack>
#include "symbole.h"
#include "lexer.h"

using namespace std;

class Etat;  // forward declaration

class Automate {
   public:
      Automate();
      ~Automate();

      bool Analyser(const string& expression);
      int GetResultat() const { return resultat; }
      void Afficher();

      // Accesseurs pour le design pattern State
      stack<Etat*>& getPilEtat()       { return pilEtat; }
      stack<Symbole*>& getPilSymbole() { return pilSymbole; }
      void setResultat(int r) { resultat = r; }
      void setAccepte()       { accepte = true; }
      void setErreur()        { erreur = true; }
      void avancerLexer()     { if (lexer) lexer->Avancer(); }

   private:
      stack<Etat*>    pilEtat;
      stack<Symbole*> pilSymbole;
      int  resultat;
      bool accepte;
      bool erreur;
      Lexer* lexer;  // positionné pendant Analyser(), null sinon
};
