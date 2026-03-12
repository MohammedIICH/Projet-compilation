#pragma once

#include "symbole.h"

class Automate;

// Classe abstraite — design pattern State.
// Chaque état concret code directement les cases de la table LALR(1).
class Etat {
   public:
      virtual ~Etat() {}
      virtual void transition(Automate& a, Symbole* s) = 0;
};

// ─── Table LALR(1) résolue ────────────────────────────────────────────────────
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
//    7  │      │  r2  │  d5  │      │  r2  │  r2  ║         ← E → E+E
//    8  │      │  r3  │  r3  │      │  r3  │  r3  ║         ← E → E*E
//    9  │      │  r4  │  r4  │      │  r4  │  r4  ║         ← E → (E)
//
//  r2 = E→E+E   r3 = E→E*E   r4 = E→(E)   r5 = E→val
// ─────────────────────────────────────────────────────────────────────────────

class Etat0 : public Etat { public: void transition(Automate& a, Symbole* s) override; };
class Etat1 : public Etat { public: void transition(Automate& a, Symbole* s) override; };
class Etat2 : public Etat { public: void transition(Automate& a, Symbole* s) override; };
class Etat3 : public Etat { public: void transition(Automate& a, Symbole* s) override; };
class Etat4 : public Etat { public: void transition(Automate& a, Symbole* s) override; };
class Etat5 : public Etat { public: void transition(Automate& a, Symbole* s) override; };
class Etat6 : public Etat { public: void transition(Automate& a, Symbole* s) override; };
class Etat7 : public Etat { public: void transition(Automate& a, Symbole* s) override; };
class Etat8 : public Etat { public: void transition(Automate& a, Symbole* s) override; };
class Etat9 : public Etat { public: void transition(Automate& a, Symbole* s) override; };
