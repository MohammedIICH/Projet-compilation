#include "automate.h"
#include <iostream>
#include <cctype>

using namespace std;

// Classe locale pour analyse par descente recursive
// Structure: addition() -> multiplication() -> primaire()
// Cette hierarchie garantit les priorites operatoires
class AnalyseurRecursif {
private:
   Lexer lexer;
   Symbole* symbole_courant;
   int resultat_expr = 0;
   
public:
   AnalyseurRecursif(const string& expr) : lexer(expr), symbole_courant(nullptr) {
      avancer();
   }

   ~AnalyseurRecursif() {
      if (symbole_courant) delete symbole_courant;
   }

   void avancer() {
      if (symbole_courant) delete symbole_courant;
      symbole_courant = lexer.Consulter();
      lexer.Avancer();
   }

   bool analyser() {
      resultat_expr = expression();
      bool ok = (symbole_courant && symbole_courant->getIdent() == FIN);
      return ok;
   }

   int getResultat() {
      return resultat_expr;
   }

private:
   int expression() {
      return addition();
   }

   // + a la priorite la plus basse -> evaluer d'abord les * depuis multiplication()
   int addition() {
      int val = multiplication();
      
      while (symbole_courant && symbole_courant->getIdent() == PLUS) {
         avancer();
         int droit = multiplication();
         val = val + droit;
      }
      
      return val;
   }

   // * a priorite haute -> evaluer d'abord les nombres depuis primaire()
   int multiplication() {
      int val = primaire();
      
      while (symbole_courant && symbole_courant->getIdent() == MULT) {
         avancer();
         int droit = primaire();
         val = val * droit;
      }
      
      return val;
   }

   // Niveau haut : nombres et expressions entre parentheses
   int primaire() {
      if (!symbole_courant) return 0;
      
      if (symbole_courant->getIdent() == INT) {
         int val = symbole_courant->getValeur();
         avancer();
         return val;
      } else if (symbole_courant->getIdent() == OPENPAR) {
         avancer();
         int val = expression();
         if (symbole_courant && symbole_courant->getIdent() == CLOSEPAR) {
            avancer();
         }
         return val;
      }
      return 0;
   }
};

Automate::Automate() : resultat(0) {
}

Automate::~Automate() {
   while (!pilEtat.empty()) {
      pilEtat.pop();
   }
   while (!pilSymbole.empty()) {
      Symbole* s = pilSymbole.top();
      pilSymbole.pop();
      delete s;
   }
}

// Lance l'analyse et evaluation de l'expression
bool Automate::Analyser(const string& expression_str) {
   AnalyseurRecursif analyseur(expression_str);
   
   if (analyseur.analyser()) {
      resultat = analyseur.getResultat();
      return true;
   } else {
      return false;
   }
}

void Automate::Afficher() {
   cout << "  * Resultat: " << resultat << endl;
}

// Methodes non utilisees (reserves pour impl LALR(1) avec table d'automate)
Action Automate::getAction(int etat, int symbole) {
   Action act;
   act.type = ERROR_ACTION;
   act.param = 0;
   return act;
}

int Automate::getGoto(int etat, int non_terminal) {
   return -1;
}

int Automate::evaluerReduction(int production) {
   return 0;
}

