#pragma once

#include <stack>
#include "symbole.h"
#include "lexer.h"

class Etat;

class Automate {
   public:
      Automate();
      ~Automate();

      bool Analyser(const std::string& expression);
      int  GetResultat() const { return resultat; }
      void Afficher();

      // Accesseurs pour le design pattern State
      std::stack<Etat*>&    getPilEtat()     { return pilEtat; }
      std::stack<Symbole*>& getPilSymbole()  { return pilSymbole; }
      void setResultat(int r) { resultat = r; }
      void setAccepte()       { accepte = true; }
      void setErreur()        { erreur  = true; }
      void avancerLexer()     { if (lexer) lexer->Avancer(); }

   private:
      std::stack<Etat*>    pilEtat;
      std::stack<Symbole*> pilSymbole;
      int   resultat;
      bool  accepte;
      bool  erreur;
      Lexer* lexer;   // non nul uniquement pendant Analyser()
};
