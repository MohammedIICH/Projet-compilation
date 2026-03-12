#pragma once

#include "symbole.h"

class Automate;  // forward declaration

// Classe abstraite de base pour tous les états LALR(1).
// Design pattern State : chaque état concret implémente transition()
// en codant directement les cases de la table LALR(1).
class Etat {
   public:
      virtual ~Etat() {}
      virtual void transition(Automate& a, Symbole* s) = 0;
      virtual int getNumero() const = 0;
};

// ─── Table LALR(1) résolue (après levée des conflits) ────────────────────────
//
//  Etat │ val  │  +   │  *   │  (   │  )   │  $   ║  E (goto)
//  ─────┼──────┼──────┼──────┼──────┼──────┼──────╫──────────
//    0  │  d3  │      │      │  d2  │      │      ║    1
//    1  │      │  d4  │  d5  │      │      │  acc ║
//    2  │  d3  │      │      │  d2  │      │      ║    6
//    3  │      │  r5  │  r5  │      │  r5  │  r5  ║         ← E → val
//    4  │  d3  │      │      │  d2  │      │      ║    7
//    5  │  d3  │      │      │  d2  │      │      ║    8
//    6  │      │  d4  │  d5  │      │  d9  │      ║
//    7  │      │  r2  │  d5  │      │  r2  │  r2  ║         ← E → E+E  (+ assoc-gauche)
//    8  │      │  r3  │  r3  │      │  r3  │  r3  ║         ← E → E*E  (* priorité)
//    9  │      │  r4  │  r4  │      │  r4  │  r4  ║         ← E → (E)
//
//  Productions : r2 = E→E+E  r3 = E→E*E  r4 = E→(E)  r5 = E→val
// ─────────────────────────────────────────────────────────────────────────────

class Etat0 : public Etat {
   public:
      virtual void transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 0; }
};

class Etat1 : public Etat {
   public:
      virtual void transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 1; }
};

class Etat2 : public Etat {
   public:
      virtual void transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 2; }
};

class Etat3 : public Etat {
   public:
      virtual void transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 3; }
};

class Etat4 : public Etat {
   public:
      virtual void transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 4; }
};

class Etat5 : public Etat {
   public:
      virtual void transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 5; }
};

class Etat6 : public Etat {
   public:
      virtual void transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 6; }
};

class Etat7 : public Etat {
   public:
      virtual void transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 7; }
};

class Etat8 : public Etat {
   public:
      virtual void transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 8; }
};

class Etat9 : public Etat {
   public:
      virtual void transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 9; }
};
