#include "lexer.h"

// Retourne le jeton courant sans l'avancer (cache avec tamppon)
Symbole * Lexer::Consulter() {
   if (!tampon) {
      if (tete >= (int)flux.length())
         tampon = new Symbole(FIN);
      else {
         switch (flux[tete]) {
            case '(':
               tampon = new Symbole(OPENPAR);
               tete++;
               break;
            case ')':
               tampon = new Symbole(CLOSEPAR);
               tete++;
               break;
            case '*':
               tampon = new Symbole(MULT);
               tete++;
               break;
            case '+':
               tampon = new Symbole(PLUS);
               tete++;
               break;
            default:
               // Accélère les entiers multi-chiffres
               if (flux[tete] <= '9' && flux[tete] >= '0') {
                  int resultat = flux[tete] - '0';
                  int i = 1;
                  while (tete + i < (int)flux.length() && flux[tete + i] <= '9' && flux[tete + i] >= '0') {
                     resultat = resultat * 10 + (flux[tete + i] - '0');
                     i++;
                  }
                  tete = tete + i;
                  tampon = new Entier(resultat);
               } else {
                  tampon = new Symbole(ERREUR);
               }
         }
      }
   }
   return tampon;
}

// Efface le jeton en cache pour prendre le suivant
void Lexer::Avancer() {
   tampon = nullptr;
}

