#pragma once

#include <string>
#include "symbole.h"

class Lexer {
   public:
      Lexer(std::string s) : flux(s), tete(0), tampon(nullptr) {}
      ~Lexer() { delete tampon; }

      Symbole* Consulter();
      void     Avancer();

   protected:
      std::string flux;
      int         tete;
      Symbole*    tampon;
};
