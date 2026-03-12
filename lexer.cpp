#include "lexer.h"

// Retourne le jeton courant sans avancer (mise en cache dans tampon)
Symbole* Lexer::Consulter() {
   if (!tampon) {
      if (tete >= (int)flux.length()) {
         tampon = new Symbole(FIN);
      } else {
         switch (flux[tete]) {
            case '(': tampon = new Symbole(OPENPAR); tete++; break;
            case ')': tampon = new Symbole(CLOSEPAR); tete++; break;
            case '+': tampon = new Symbole(PLUS);    tete++; break;
            case '*': tampon = new Symbole(MULT);    tete++; break;
            default:
               if (flux[tete] >= '0' && flux[tete] <= '9') {
                  int val = flux[tete] - '0';
                  int i   = 1;
                  while (tete + i < (int)flux.length()
                         && flux[tete + i] >= '0' && flux[tete + i] <= '9') {
                     val = val * 10 + (flux[tete + i] - '0');
                     i++;
                  }
                  tete += i;
                  tampon = new Entier(val);
               } else {
                  tampon = new Symbole(ERREUR);
               }
         }
      }
   }
   return tampon;
}

// Efface le tampon pour passer au jeton suivant
void Lexer::Avancer() {
   tampon = nullptr;
}
