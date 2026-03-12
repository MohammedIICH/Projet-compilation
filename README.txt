TP Compilation — Analyseur syntaxique LALR(1) avec évaluation
=============================================================


1. COMPILATION
──────────────

   $ make          →  compile et produit l'exécutable "analyseur"
   $ make run      →  compile puis exécute
   $ make clean    →  supprime les fichiers objets et l'exécutable


2. UTILISATION
──────────────

   Mode test intégré (10 expressions prédéfinies) :

      $ ./analyseur

   Mode ligne de commande (une expression en argument) :

      $ g++ -Wall -std=c++11 -o tester tester.cpp symbole.o lexer.o etat.o automate.o
      $ ./tester "1+2*3"
      7


3. STRUCTURE DU PROJET
──────────────────────

   symbole.h / symbole.cpp   —  Hiérarchie des symboles (Symbole, Entier, Expression)
   lexer.h   / lexer.cpp     —  Analyseur lexical (tokenisation de la chaîne d'entrée)
   etat.h    / etat.cpp      —  Design pattern State : classes Etat0 … Etat9
   automate.h / automate.cpp —  Automate LALR(1) : boucle d'analyse + évaluation
   main.cpp                  —  Programme principal avec suite de tests
   tester.cpp                —  Point d'entrée pour les tests de l'équipe enseignante
   Makefile                  —  Règles de compilation


4. ARCHITECTURE — AUTOMATE LALR(1) + PATTERN STATE
───────────────────────────────────────────────────

   Grammaire (ambiguë G1) :
      1)  E' → E
      2)  E  → E + E   (associativité gauche)
      3)  E  → E * E   (associativité gauche, priorité sur +)
      4)  E  → ( E )
      5)  E  → val

   L'Automate maintient deux piles :
      • pilEtat    : stack<Etat*>    — pile des états courants
      • pilSymbole : stack<Symbole*> — pile des symboles/valeurs

   Boucle principale dans Automate::Analyser() :
      tant que non accepté et non erreur :
         pilEtat.top()->transition(automate, lexer.Consulter())

   Chaque état concret (EtatN) implémente transition() selon la table LALR(1) :

      — Décalage (dN)  : empile le symbole, avance le lexer, empile EtatN
      — Réduction (rX) : dépile |rhs| symboles + états, calcule la valeur,
                         empile une Expression, appelle GOTO sur l'état suivant
      — Acceptation    : enregistre le résultat, arrête la boucle
      — GOTO (E reçu)  : empile le bon état de destination

   Table LALR(1) résolue :

   Etat │ val  │  +   │  *   │  (   │  )   │  $   ║ E (goto)
   ─────┼──────┼──────┼──────┼──────┼──────┼──────╫─────────
     0  │  d3  │      │      │  d2  │      │      ║    1
     1  │      │  d4  │  d5  │      │      │  acc ║
     2  │  d3  │      │      │  d2  │      │      ║    6
     3  │      │  r5  │  r5  │      │  r5  │  r5  ║
     4  │  d3  │      │      │  d2  │      │      ║    7
     5  │  d3  │      │      │  d2  │      │      ║    8
     6  │      │  d4  │  d5  │      │  d9  │      ║
     7  │      │  r2  │  d5  │      │  r2  │  r2  ║  (* priorité sur +)
     8  │      │  r3  │  r3  │      │  r3  │  r3  ║  (* assoc. gauche)
     9  │      │  r4  │  r4  │      │  r4  │  r4  ║

   Productions :  r2 = E→E+E   r3 = E→E*E   r4 = E→(E)   r5 = E→val

   Résolution des conflits :
      • État 7, + : conflit d4/r2  →  r2  (associativité gauche de +)
      • État 7, * : conflit d5/r2  →  d5  (priorité de * sur +)
      • État 8, + : conflit d4/r3  →  r3  (priorité de * sur +)
      • État 8, * : conflit d5/r3  →  r3  (associativité gauche de *)


5. EXEMPLES DE TEST
───────────────────

   42              →   42
   1+2             →    3
   2*3             →    6
   1+2*3           →    7   (priorité : 1+(2*3))
   2*3+4           →   10   ((2*3)+4)
   (1+2)*3         →    9
   3*(2+4)         →   18
   10+20+30        →   60   (associativité gauche)
   (5+5)*(2+8)     →  100
   ((3+4)*5)       →   35


6. NOTES
────────

   • Compilation sans warnings : g++ -Wall -std=c++11
   • Gestion mémoire : chaque symbole est supprimé après réduction ;
     les états se suppriment eux-mêmes après dépilage (delete this)
   • L'évaluation est réalisée à l'intérieur du programme d'analyse
     (lors de chaque réduction), conformément à l'énoncé
