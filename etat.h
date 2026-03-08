#pragma once

#include "symbole.h"

class Automate;  // Forward declaration

class Etat {
   public:
      virtual ~Etat() {}
      
      // Traite un symbole et décide de l'action à effectuer
      virtual Etat* transition(Automate& a, Symbole* s) = 0;
      
      // Retourne le numéro de l'état
      virtual int getNumero() const = 0;
};

// États LALR(1) pour la grammaire:
// E' → E
// E → E + E  (production 1)
// E → E * E  (production 2)
// E → (E)    (production 3)
// E → val    (production 4)

class Etat0 : public Etat {
   public:
      virtual Etat* transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 0; }
};

class Etat1 : public Etat {
   public:
      virtual Etat* transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 1; }
};

class Etat2 : public Etat {
   public:
      virtual Etat* transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 2; }
};

class Etat3 : public Etat {
   public:
      virtual Etat* transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 3; }
};

class Etat4 : public Etat {
   public:
      virtual Etat* transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 4; }
};

class Etat5 : public Etat {
   public:
      virtual Etat* transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 5; }
};

class Etat6 : public Etat {
   public:
      virtual Etat* transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 6; }
};

class Etat7 : public Etat {
   public:
      virtual Etat* transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 7; }
};

class Etat8 : public Etat {
   public:
      virtual Etat* transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 8; }
};

class Etat9 : public Etat {
   public:
      virtual Etat* transition(Automate& a, Symbole* s);
      virtual int getNumero() const { return 9; }
};
