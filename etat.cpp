#include "etat.h"
#include "automate.h"
#include <iostream>
#include <cstdlib>

using namespace std;

// Fonctions helpers pour les reductions
int evaluerReduction(int production, stack<Symbole*>& pilSymbole) {
   switch (production) {
      case 1: {  // E → E + E
         if (pilSymbole.size() >= 3) {
            Symbole* e2 = pilSymbole.top(); pilSymbole.pop();
            Symbole* op = pilSymbole.top(); pilSymbole.pop();
            Symbole* e1 = pilSymbole.top(); pilSymbole.pop();
            int res = e1->getValeur() + e2->getValeur();
            delete e1; delete op; delete e2;
            return res;
         }
         return 0;
      }
      case 2: {  // E → E * E
         if (pilSymbole.size() >= 3) {
            Symbole* e2 = pilSymbole.top(); pilSymbole.pop();
            Symbole* op = pilSymbole.top(); pilSymbole.pop();
            Symbole* e1 = pilSymbole.top(); pilSymbole.pop();
            int res = e1->getValeur() * e2->getValeur();
            delete e1; delete op; delete e2;
            return res;
         }
         return 0;
      }
      case 3: {  // E → (E)
         if (pilSymbole.size() >= 3) {
            Symbole* close = pilSymbole.top(); pilSymbole.pop();
            Symbole* e = pilSymbole.top(); pilSymbole.pop();
            Symbole* open = pilSymbole.top(); pilSymbole.pop();
            int res = e->getValeur();
            delete open; delete e; delete close;
            return res;
         }
         return 0;
      }
      case 4: {  // E → val
         if (!pilSymbole.empty()) {
            Symbole* val = pilSymbole.top(); pilSymbole.pop();
            int res = val->getValeur();
            delete val;
            return res;
         }
         return 0;
      }
      default:
         return 0;
   }
}

// État 0: État de départ
Etat* Etat0::transition(Automate& a, Symbole* s) {
   stack<Symbole*>& pilSymbole = a.getPilSymbole();

   switch (s->getIdent()) {
      case INT:  // val → décaler État 1
         pilSymbole.push(s);
         return new Etat1();
      case OPENPAR:  // ( → décaler État 4
         pilSymbole.push(s);
         return new Etat4();
      case FIN:  // $ → Erreur ou donner priorité
         cerr << "Erreur: FIN en état 0 sans expression" << endl;
         return nullptr;
      default:
         cerr << "Erreur: symbole non attendu en état 0" << endl;
         return nullptr;
   }
}

// État 1: Après lecture de INT (val)
Etat* Etat1::transition(Automate& a, Symbole* s) {
   stack<Etat*>& pilEtat = a.getPilEtat();
   stack<Symbole*>& pilSymbole = a.getPilSymbole();

   // Production: E → val (réduction)
   int valeur = evaluerReduction(4, pilSymbole);  // Production 4
   
   pilEtat.pop();  // Dépile Etat1
   Etat* prevEtat = pilEtat.top();
   
   // Crée une expression et la pousse
   Expression* expr = new Expression(valeur);
   pilSymbole.push(expr);
   
   // Demande au state précédent de traiter le non-terminal E
   return prevEtat->transition(a, new Symbole(E));
}


// État 2: Après shift de E depuis État 0
Etat* Etat2::transition(Automate& a, Symbole* s) {
   stack<Symbole*>& pilSymbole = a.getPilSymbole();

   switch (s->getIdent()) {
      case PLUS:  // + → décaler État 2
         pilSymbole.push(s);
         return new Etat2();
      case MULT:  // * → décaler État 3
         pilSymbole.push(s);
         return new Etat3();
      case FIN:  // $ → accepter (si c'est l'axiome)
         if (pilSymbole.size() >= 1) {
            Symbole* top = pilSymbole.top();
            if (top->getIdent() == E) {
               a.setResultat(top->getValeur());
               return nullptr;  // Signal d'acceptation
            }
         }
         cerr << "Erreur: FIN sans expression complète" << endl;
         return nullptr;
      default:
         cerr << "Erreur: symbole non attendu en état 2" << endl;
         return nullptr;
   }
}

// État 3: Après shift de MULT
Etat* Etat3::transition(Automate& a, Symbole* s) {
   stack<Symbole*>& pilSymbole = a.getPilSymbole();

   switch (s->getIdent()) {
      case INT:  // val → décaler État 1
         pilSymbole.push(s);
         return new Etat1();
      case OPENPAR:  // ( → décaler État 4
         pilSymbole.push(s);
         return new Etat4();
      default:
         cerr << "Erreur: symbole non attendu en état 3" << endl;
         return nullptr;
   }
}

// État 4: Après OPENPAR
Etat* Etat4::transition(Automate& a, Symbole* s) {
   stack<Symbole*>& pilSymbole = a.getPilSymbole();

   switch (s->getIdent()) {
      case INT:  // val → décaler État 1
         pilSymbole.push(s);
         return new Etat1();
      case OPENPAR:  // ( → décaler État 4
         pilSymbole.push(s);
         return new Etat4();
      case PLUS:  // + → décaler État 2
         pilSymbole.push(s);
         return new Etat2();
      default:
         cerr << "Erreur: symbole non attendu en état 4" << endl;
         return nullptr;
   }
}

// État 5: Après réduction E dans état 4
Etat* Etat5::transition(Automate& a, Symbole* s) {
   stack<Etat*>& pilEtat = a.getPilEtat();
   stack<Symbole*>& pilSymbole = a.getPilSymbole();

   switch (s->getIdent()) {
      case CLOSEPAR: {  // ) → décaler État 5 (et puis réductionE → (E))
         pilSymbole.push(s);
         // Production: E → (E) (réduction)
         int valeur = evaluerReduction(3, pilSymbole);
         
         pilEtat.pop();  // Dépile Etat5
         pilEtat.pop();  // Dépile Etat4
         Etat* prevEtat = pilEtat.top();
         
         Expression* expr = new Expression(valeur);
         pilSymbole.push(expr);
         
         return prevEtat->transition(a, new Symbole(E));
      }
      case PLUS: {  // + → décaler État 2 si E attendu
         pilSymbole.push(s);
         return new Etat2();
      }
      case MULT: {  // * → décaler État 3
         pilSymbole.push(s);
         return new Etat3();
      }
      default:
         cerr << "Erreur: symbole non attendu en état 5" << endl;
         return nullptr;
   }
}

// État 6: Après shift de + avec priorité
Etat* Etat6::transition(Automate& a, Symbole* s) {
   stack<Etat*>& pilEtat = a.getPilEtat();
   stack<Symbole*>& pilSymbole = a.getPilSymbole();

   // Production: E → E + E (réduction avec associativité gauche)
   int valeur = evaluerReduction(1, pilSymbole);
   
   pilEtat.pop();  // Dépile l'état
   Etat* prevEtat = pilEtat.top();
   
   Expression* expr = new Expression(valeur);
   pilSymbole.push(expr);
   
   return prevEtat->transition(a, new Symbole(E));
}

// État 7: Après shift de * avec priorité  
Etat* Etat7::transition(Automate& a, Symbole* s) {
   stack<Etat*>& pilEtat = a.getPilEtat();
   stack<Symbole*>& pilSymbole = a.getPilSymbole();

   // Production: E → E * E (réduction avec associativité gauche)
   int valeur = evaluerReduction(2, pilSymbole);
   
   pilEtat.pop();  // Dépile l'état
   Etat* prevEtat = pilEtat.top();
   
   Expression* expr = new Expression(valeur);
   pilSymbole.push(expr);
   
   return prevEtat->transition(a, new Symbole(E));
}

// État 8: Après E dans parenthèses
Etat* Etat8::transition(Automate& a, Symbole* s) {
   stack<Etat*>& pilEtat = a.getPilEtat();
   stack<Symbole*>& pilSymbole = a.getPilSymbole();

   switch (s->getIdent()) {
      case CLOSEPAR: {  // ) → production E → (E)
         pilSymbole.push(s);
         // Réduction
         int valeur = evaluerReduction(3, pilSymbole);
         
         pilEtat.pop();  // État 8
         pilEtat.pop();  // État 4
         Etat* prevEtat = pilEtat.top();
         
         Expression* expr = new Expression(valeur);
         pilSymbole.push(expr);
         
         return prevEtat->transition(a, new Symbole(E));
      }
      case PLUS: {
         pilSymbole.push(s);
         return new Etat2();
      }
      case MULT: {
         pilSymbole.push(s);
         return new Etat3();
      }
      default:
         cerr << "Erreur: symbole non attendu en état 8" << endl;
         return nullptr;
   }
}

// État 9: Erreur/Réduction finale
Etat* Etat9::transition(Automate& a, Symbole* s) {
   cerr << "Erreur: état 9 atteint de manière inattendue" << endl;
   return nullptr;
}
