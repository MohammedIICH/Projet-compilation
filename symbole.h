#pragma once

#include <string>

enum Identificateurs { OPENPAR, CLOSEPAR, PLUS, MULT, INT, FIN, ERREUR, E, E_PRIME };

class Symbole {
   public:
      Symbole(int i) : ident(i) {}
      virtual ~Symbole() {}
      operator int() const { return ident; }
      int getIdent() const { return ident; }
      virtual void Affiche();
      virtual int getValeur() const { return -1; }

   protected:
      int ident;
};

class Entier : public Symbole {
   public:
      Entier(int v) : Symbole(INT), valeur(v) {}
      void Affiche() override;
      int getValeur() const override { return valeur; }

   protected:
      int valeur;
};

class Expression : public Symbole {
   public:
      Expression(int v) : Symbole(E), valeur(v) {}
      void Affiche() override;
      int getValeur() const override { return valeur; }

   protected:
      int valeur;
};
