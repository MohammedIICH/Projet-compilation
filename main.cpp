#include <iostream>
#include "automate.h"

using namespace std;

static void lancerTests() {
   const string expressions[] = {
      "42", "1+2", "2*3", "1+2*3", "2*3+4",
      "(1+2)*3", "3*(2+4)", "10+20+30", "(5+5)*(2+8)", "((3+4)*5)"
   };

   cout << "═══════════════════════════════════════════════════════════" << endl;
   for (const string& expr : expressions) {
      cout << "Expression: " << expr << endl;
      Automate a;
      if (a.Analyser(expr))
         cout << "  ✓ Résultat: " << a.GetResultat() << endl;
      else
         cout << "  ✗ Erreur syntaxique" << endl;
      cout << endl;
   }
   cout << "═══════════════════════════════════════════════════════════" << endl;
}

static void modeInteractif() {
   cout << "═══════════════════════════════════════════════════════════" << endl;
   cout << "Entrez une expression (ou 'q' pour quitter) :" << endl << endl;

   string expr;
   while (true) {
      cout << "  > ";
      if (!getline(cin, expr) || expr == "q" || expr == "quit")
         break;
      if (expr.empty())
         continue;

      Automate a;
      if (a.Analyser(expr))
         cout << "  ✓ Résultat: " << a.GetResultat() << endl;
      else
         cout << "  ✗ Erreur syntaxique" << endl;
      cout << endl;
   }
   cout << "═══════════════════════════════════════════════════════════" << endl;
}

int main() {
   cout << "╔════════════════════════════════════════════════════════════╗" << endl;
   cout << "║      Analyseur Syntaxique LALR(1) & Évaluateur            ║" << endl;
   cout << "║     Grammaire avec Priorité et Associativité              ║" << endl;
   cout << "╚════════════════════════════════════════════════════════════╝" << endl << endl;

   cout << "Grammaire:" << endl;
   cout << "  E' → E" << endl;
   cout << "  E  → E + E  (associativité gauche)" << endl;
   cout << "  E  → E * E  (associativité gauche, priorité > +)" << endl;
   cout << "  E  → ( E )" << endl;
   cout << "  E  → val"   << endl << endl;

   cout << "Que voulez-vous faire ?" << endl;
   cout << "  1. Lancer les tests automatiques" << endl;
   cout << "  2. Entrer une expression manuellement" << endl;
   cout << "  > ";

   string choix;
   getline(cin, choix);
   cout << endl;

   if (choix == "1")
      lancerTests();
   else if (choix == "2")
      modeInteractif();
   else
      cout << "Choix invalide." << endl;

   return 0;
}
