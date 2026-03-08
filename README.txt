1. COMPILATION
   Pour compiler le projet:

      $ make

   Cela genere l'executable "analyseur". 

   Pour nettoyer les fichiers objets et l'executable:

      $ make clean

   Pour compiler et executer directement:

      $ make run


2. STRUCTURE DU PROJET

   Fichiers sources:
   - symbole.h/cpp     : Definition des symboles (terminaux et non-terminaux)
   - lexer.h/cpp       : Analyseur lexical - tokenisation de l'entree
   - automate.h/cpp    : Analyseur syntaxique par descente recursive
   - etat.h/cpp        : Architecture Pattern State (documentation/alternative)
   - main.cpp          : Programme principal avec tests

   Makefile            : Automatisation de la compilation


3. FONCTIONNEMENT

   L'analyseur utilise une approche par descente recursive avec evaluation
   on-the-fly des expressions. La grammaire acceptee est:

      E  -> E + E  (associativite gauche)
      E  -> E * E  (associativite gauche)
      E  -> (E)
      E  -> entier

   Exemple: 1+2*3 evalue a 7 (respecte des priorites)


4. PRIORITES OPERATOIRES

   L'ordre de priorite est assure par la structure hierarchique de la descente
   recursive:

   - addition()  : niveau bas (+ a la priorite la plus basse)
   - multiplication() : niveau intermediaire (* a priorite plus haute)
   - primaire()  : niveau haut (nombres et expressions entre parentheses)

   Lors de l'appel addition() -> multiplication() -> primaire(), les
   expressions avec * sont evaluees en premier, garantissant la priorite.

   Associativite garantie par les boucles while dans addition() et
   multiplication() qui lient a gauche les operateurs du meme niveau.


5. EXEMPLES DE TEST

   42              -> 42
   1+2             -> 3
   2*3             -> 6
   1+2*3           -> 7         (1+(2*3), pas 9)
   2*3+4           -> 10        ((2*3)+4)
   (1+2)*3         -> 9
   3*(2+4)         -> 18
   10+20+30        -> 60
   (5+5)*(2+8)     -> 100
   ((3+4)*5)       -> 35


6. ARCHITECTURE ALTERNATIVE (NON COMPILEE)

   L'architecture Pattern State est documentee dans etat.h/etat.cpp.
   Cette approche utilise une table LALR(1) avec 10 etats (Etat0 a Etat9)
   et materialise l'automate par des classes d'etats concrets.

   Raison de ne pas compiler: la descente recursive offre une solution plus
   simple et directe pour cette grammaire simple, tout en garantissant
   l'ordre d'evaluation correct.


7. NOTES DE CONFORMITE

   - Compilation sans warnings: g++ -Wall -std=c++11
   - Nettoyage memoire: suppression des symboles apres reduction
   - Fichiers: uniquement sources (.h, .cpp) et Makefile dans l'archive
   - Commentaires: concis en francais, expliquant le "pourquoi"
