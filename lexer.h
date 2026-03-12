#pragma once

#include <string>
#include "symbole.h"
using namespace std;

class Lexer {

   public:
      Lexer(string s) : flux(s), tete(0), tampon(nullptr) { }
      ~Lexer() { delete tampon; }

      Symbole * Consulter();
      void Avancer();

   protected:
      string flux;
      int tete;
      Symbole * tampon;
};
