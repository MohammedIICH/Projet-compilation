#include "etat.h"
#include "automate.h"
#include <iostream>

using namespace std;

// ─── Conventions ────────────────────────────────────────────────────────────
//
//  • Décalage (shift dN) : empiler le symbole courant sur pilSymbole,
//    avancer le lexer, empiler le nouvel état sur pilEtat.
//
//  • Réduction (rX) : dépiler |rhs| symboles + |rhs| états, calculer la
//    valeur, empiler une Expression sur pilSymbole, puis appeler GOTO en
//    passant new Symbole(E) à l'état maintenant au sommet de pilEtat.
//
//  • GOTO (réception de E) : empiler le bon état, supprimer le symbole E
//    temporaire.  Ne PAS avancer le lexer (le terminal courant reste le même).
//
//  • delete this : chaque état se supprime après s'être dépilé.  Après
//    « pilEtat.pop(); delete this; », on ne touche plus à aucun membre ;
//    on n'utilise que les références locales (pilEtat, pilSymbole) qui
//    pointent sur l'Automate, toujours vivant.
// ─────────────────────────────────────────────────────────────────────────────


// ── Etat0 : état initial ─────────────────────────────────────────────────────
// val→d3 │ (→d2 │ E→1
void Etat0::transition(Automate& a, Symbole* s) {
   stack<Etat*>&    pe = a.getPilEtat();
   stack<Symbole*>& ps = a.getPilSymbole();

   switch (s->getIdent()) {
      case INT:     // décalage vers Etat3
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat3());
         break;
      case OPENPAR: // décalage vers Etat2
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat2());
         break;
      case E:       // GOTO 1
         delete s;
         pe.push(new Etat1());
         break;
      default:
         cerr << "Erreur syntaxique (etat 0)" << endl;
         a.setErreur();
   }
}

// ── Etat1 : E lu au niveau racine ────────────────────────────────────────────
// +→d4 │ *→d5 │ $→accepter
void Etat1::transition(Automate& a, Symbole* s) {
   stack<Etat*>&    pe = a.getPilEtat();
   stack<Symbole*>& ps = a.getPilSymbole();

   switch (s->getIdent()) {
      case PLUS:    // décalage vers Etat4
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat4());
         break;
      case MULT:    // décalage vers Etat5
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat5());
         break;
      case FIN:     // ACCEPTER
         a.setResultat(ps.top()->getValeur());
         a.setAccepte();
         break;
      default:
         cerr << "Erreur syntaxique (etat 1)" << endl;
         a.setErreur();
   }
}

// ── Etat2 : après ( ──────────────────────────────────────────────────────────
// val→d3 │ (→d2 │ E→6
void Etat2::transition(Automate& a, Symbole* s) {
   stack<Etat*>&    pe = a.getPilEtat();
   stack<Symbole*>& ps = a.getPilSymbole();

   switch (s->getIdent()) {
      case INT:
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat3());
         break;
      case OPENPAR:
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat2());
         break;
      case E:       // GOTO 6
         delete s;
         pe.push(new Etat6());
         break;
      default:
         cerr << "Erreur syntaxique (etat 2)" << endl;
         a.setErreur();
   }
}

// ── Etat3 : après val ────────────────────────────────────────────────────────
// r5 pour tout : +, *, ), $   →   E → val
void Etat3::transition(Automate& a, Symbole* s) {
   stack<Etat*>&    pe = a.getPilEtat();
   stack<Symbole*>& ps = a.getPilSymbole();

   switch (s->getIdent()) {
      case PLUS:
      case MULT:
      case CLOSEPAR:
      case FIN: {
         // Réduction r5 : E → val  (longueur 1)
         Symbole* val = ps.top(); ps.pop();
         int v = val->getValeur();
         delete val;

         pe.pop(); delete this;   // dépile et supprime cet état

         ps.push(new Expression(v));
         pe.top()->transition(a, new Symbole(E));   // GOTO
         break;
      }
      default:
         cerr << "Erreur syntaxique (etat 3)" << endl;
         a.setErreur();
   }
}

// ── Etat4 : après E + ────────────────────────────────────────────────────────
// val→d3 │ (→d2 │ E→7
void Etat4::transition(Automate& a, Symbole* s) {
   stack<Etat*>&    pe = a.getPilEtat();
   stack<Symbole*>& ps = a.getPilSymbole();

   switch (s->getIdent()) {
      case INT:
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat3());
         break;
      case OPENPAR:
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat2());
         break;
      case E:       // GOTO 7
         delete s;
         pe.push(new Etat7());
         break;
      default:
         cerr << "Erreur syntaxique (etat 4)" << endl;
         a.setErreur();
   }
}

// ── Etat5 : après E * ────────────────────────────────────────────────────────
// val→d3 │ (→d2 │ E→8
void Etat5::transition(Automate& a, Symbole* s) {
   stack<Etat*>&    pe = a.getPilEtat();
   stack<Symbole*>& ps = a.getPilSymbole();

   switch (s->getIdent()) {
      case INT:
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat3());
         break;
      case OPENPAR:
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat2());
         break;
      case E:       // GOTO 8
         delete s;
         pe.push(new Etat8());
         break;
      default:
         cerr << "Erreur syntaxique (etat 5)" << endl;
         a.setErreur();
   }
}

// ── Etat6 : après ( E ────────────────────────────────────────────────────────
// +→d4 │ *→d5 │ )→d9
void Etat6::transition(Automate& a, Symbole* s) {
   stack<Etat*>&    pe = a.getPilEtat();
   stack<Symbole*>& ps = a.getPilSymbole();

   switch (s->getIdent()) {
      case PLUS:
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat4());
         break;
      case MULT:
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat5());
         break;
      case CLOSEPAR:  // décalage vers Etat9
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat9());
         break;
      default:
         cerr << "Erreur syntaxique (etat 6)" << endl;
         a.setErreur();
   }
}

// ── Etat7 : après E + E ──────────────────────────────────────────────────────
// +→r2 │ *→d5 (priorité *>+) │ )→r2 │ $→r2   →   E → E + E
void Etat7::transition(Automate& a, Symbole* s) {
   stack<Etat*>&    pe = a.getPilEtat();
   stack<Symbole*>& ps = a.getPilSymbole();

   switch (s->getIdent()) {
      case MULT:    // décalage d5 : * a priorité sur + (conflit d5/r2 → d5)
         ps.push(s);
         a.avancerLexer();
         pe.push(new Etat5());
         break;
      case PLUS:
      case CLOSEPAR:
      case FIN: {
         // Réduction r2 : E → E + E  (longueur 3, associativité gauche)
         Symbole* e2 = ps.top(); ps.pop();
         Symbole* op = ps.top(); ps.pop();
         Symbole* e1 = ps.top(); ps.pop();
         int v = e1->getValeur() + e2->getValeur();
         delete e1; delete op; delete e2;

         // Dépiler 3 états : Etat7 (this), Etat4, et l'état précédant E
         pe.pop(); delete this;
         Etat* s4   = pe.top(); pe.pop(); delete s4;
         Etat* prev = pe.top(); pe.pop(); delete prev;

         ps.push(new Expression(v));
         pe.top()->transition(a, new Symbole(E));   // GOTO
         break;
      }
      default:
         cerr << "Erreur syntaxique (etat 7)" << endl;
         a.setErreur();
   }
}

// ── Etat8 : après E * E ──────────────────────────────────────────────────────
// +→r3 │ *→r3 │ )→r3 │ $→r3   →   E → E * E
void Etat8::transition(Automate& a, Symbole* s) {
   stack<Etat*>&    pe = a.getPilEtat();
   stack<Symbole*>& ps = a.getPilSymbole();

   switch (s->getIdent()) {
      case PLUS:
      case MULT:
      case CLOSEPAR:
      case FIN: {
         // Réduction r3 : E → E * E  (longueur 3)
         Symbole* e2 = ps.top(); ps.pop();
         Symbole* op = ps.top(); ps.pop();
         Symbole* e1 = ps.top(); ps.pop();
         int v = e1->getValeur() * e2->getValeur();
         delete e1; delete op; delete e2;

         // Dépiler 3 états : Etat8 (this), Etat5, et l'état précédant E
         pe.pop(); delete this;
         Etat* s5   = pe.top(); pe.pop(); delete s5;
         Etat* prev = pe.top(); pe.pop(); delete prev;

         ps.push(new Expression(v));
         pe.top()->transition(a, new Symbole(E));   // GOTO
         break;
      }
      default:
         cerr << "Erreur syntaxique (etat 8)" << endl;
         a.setErreur();
   }
}

// ── Etat9 : après ( E ) ──────────────────────────────────────────────────────
// +→r4 │ *→r4 │ )→r4 │ $→r4   →   E → ( E )
void Etat9::transition(Automate& a, Symbole* s) {
   stack<Etat*>&    pe = a.getPilEtat();
   stack<Symbole*>& ps = a.getPilSymbole();

   switch (s->getIdent()) {
      case PLUS:
      case MULT:
      case CLOSEPAR:
      case FIN: {
         // Réduction r4 : E → ( E )  (longueur 3)
         Symbole* close = ps.top(); ps.pop();
         Symbole* e     = ps.top(); ps.pop();
         Symbole* open  = ps.top(); ps.pop();
         int v = e->getValeur();
         delete close; delete e; delete open;

         // Dépiler 3 états : Etat9 (this), Etat6, Etat2
         pe.pop(); delete this;
         Etat* s6 = pe.top(); pe.pop(); delete s6;
         Etat* s2 = pe.top(); pe.pop(); delete s2;

         ps.push(new Expression(v));
         pe.top()->transition(a, new Symbole(E));   // GOTO
         break;
      }
      default:
         cerr << "Erreur syntaxique (etat 9)" << endl;
         a.setErreur();
   }
}
