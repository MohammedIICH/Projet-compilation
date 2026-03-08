#include <iostream>
#include "automate.h"

using namespace std;

int main(void) {
   // Suite de tests couvrant cas nominaux et expressions complexes
   string expressions[] = {
      "42",
      "1+2",
      "2*3",
      "1+2*3",
      "2*3+4",
      "(1+2)*3",
      "3*(2+4)",
      "10+20+30",
      "(5+5)*(2+8)",
      "((3+4)*5)"
   };

   cout << "╔════════════════════════════════════════════════════════════╗" << endl;
   cout << "║      Analyseur Syntaxique LALR(1) & Évaluateur            ║" << endl;
   cout << "║     Grammaire avec Priorité et Associativité              ║" << endl;
   cout << "╚════════════════════════════════════════════════════════════╝" << endl << endl;
   
   cout << "Grammaire:" << endl;
   cout << "  E' → E" << endl;
   cout << "  E → E + E  (* associativité gauche)" << endl;
   cout << "  E → E * E  (* associativité gauche, priorité > +)" << endl;
   cout << "  E → (E)" << endl;
   cout << "  E → val" << endl << endl;

   cout << "═══════════════════════════════════════════════════════════" << endl;

   for (const string& expr : expressions) {
      cout << "Expression: " << expr << endl;
      
      Automate a;
      if (a.Analyser(expr)) {
         cout << "  ✓ Résultat: " << a.GetResultat() << endl;
      } else {
         cout << "  ✗ Erreur: Analyse échouée" << endl;
      }
      cout << endl;
   }

   cout << "═══════════════════════════════════════════════════════════" << endl;
   cout << "Analyse terminée." << endl;

   return 0;
}

