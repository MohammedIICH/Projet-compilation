#pragma once

#include <stack>
#include <vector>
#include "symbole.h"
#include "lexer.h"

using namespace std;

// Types d'actions LALR(1)
enum ActionType { SHIFT, REDUCE, ACCEPT, ERROR_ACTION };

struct Action {
   ActionType type;
   int param;  // numéro d'état pour SHIFT, numéro de production pour REDUCE
};

class Automate {
   public:
      Automate();
      ~Automate();
      
      bool Analyser(const string& expression);
      int GetResultat() const { return resultat; }
      void Afficher();

   private:
      stack<int> pilEtat;
      stack<Symbole*> pilSymbole;
      int resultat;
      
      // Table d'analyse LALR(1)
      Action getAction(int etat, int symbole);
      int getGoto(int etat, int non_terminal);
      
      // Évaluation des réductions
      int evaluerReduction(int production);
};


