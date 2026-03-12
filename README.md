# TP Compilation — Analyseur syntaxique LALR(1)

Analyseur syntaxique ascendant avec évaluation d'expressions arithmétiques,
implémenté en C++11 via l'automate LALR(1) et le **design pattern State**.

---

## Table des matières

1. [Compilation et utilisation](#1-compilation-et-utilisation)
2. [Structure des fichiers](#2-structure-des-fichiers)
3. [Analyse lexicale — Lexer](#3-analyse-lexicale--lexer)
4. [Hiérarchie des symboles](#4-hiérarchie-des-symboles)
5. [Grammaire et théorie LALR(1)](#5-grammaire-et-théorie-lalr1)
6. [Table LALR(1) et résolution des conflits](#6-table-lalr1-et-résolution-des-conflits)
7. [Design pattern State — les états](#7-design-pattern-state--les-états)
8. [L'automate — boucle principale](#8-lautomate--boucle-principale)
9. [Programmes d'entrée](#9-programmes-dentrée)
10. [Suite de tests](#10-suite-de-tests)
11. [Bilan et résultats](#11-bilan-et-résultats)

---

## 1. Compilation et utilisation

### Commandes make

```bash
make          # compile → produit l'exécutable "analyseur"
make run      # compile puis exécute "analyseur"
make test     # compile et exécute la suite de tests complète
make clean    # supprime tous les fichiers objets, executables et binaires de test
```

### Lancer le programme principal

```bash
./analyseur
```

Un menu s'affiche au démarrage :

```
╔════════════════════════════════════════════════════════════╗
║      Analyseur Syntaxique LALR(1) & Évaluateur            ║
║     Grammaire avec Priorité et Associativité              ║
╚════════════════════════════════════════════════════════════╝

  1. Lancer les tests automatiques
  2. Entrer une expression manuellement
  >
```

**Option 1 — Tests automatiques** : exécute 10 expressions prédéfinies et affiche leurs résultats.

**Option 2 — Mode interactif** : entrez vos propres expressions une par une. Tapez `q` pour quitter.

```
  > 1+2*3
  ✓ Résultat: 7
  > (5+3)*2
  ✓ Résultat: 16
  > +1
  ✗ Erreur syntaxique
  > q
```

### Mode ligne de commande (tester.cpp)

Compile séparément le binaire `tester` pour les tests de l'équipe enseignante :

```bash
g++ -Wall -std=c++11 -o tester tester.cpp symbole.o lexer.o etat.o automate.o
./tester "1+2*3"   # affiche : 7
```

---

## 2. Structure des fichiers

```
Projet-compilation/
├── Makefile          — règles de compilation
├── symbole.h/cpp     — hiérarchie des symboles (Symbole, Entier, Expression)
├── lexer.h/cpp       — analyseur lexical (tokenisation)
├── etat.h/cpp        — design pattern State : Etat0 … Etat9
├── automate.h/cpp    — automate LALR(1), boucle principale
├── main.cpp          — programme interactif avec menu
├── tester.cpp        — point d'entrée CLI pour les tests enseignants
└── tests.cpp         — suite de tests automatisée (123 tests)
```

### Rôle de chaque fichier

| Fichier | Responsabilité |
|---|---|
| `symbole.h/cpp` | Définit les terminaux et non-terminaux manipulés par la pile de symboles |
| `lexer.h/cpp` | Découpe la chaîne d'entrée en jetons (tokens) un par un |
| `etat.h/cpp` | Encode toute la table LALR(1) via le pattern State (10 classes) |
| `automate.h/cpp` | Gère les deux piles, pilote la boucle d'analyse, stocke le résultat |
| `main.cpp` | Interface utilisateur : menu, mode interactif, tests visuels |
| `tester.cpp` | Interface CLI minimaliste : prend l'expression en argument, retourne le résultat |
| `tests.cpp` | Suite de 123 tests organisés en 12 catégories, avec compteur de réussite |

---

## 3. Analyse lexicale — Lexer

Le `Lexer` reçoit la chaîne d'entrée complète et la parcourt caractère par caractère en maintenant un **tampon d'un seul jeton** (pattern look-ahead d'un symbole).

### Interface publique

```cpp
Symbole* Consulter();  // retourne le jeton courant sans avancer
void     Avancer();    // vide le tampon → prochain appel à Consulter() lit le jeton suivant
```

### Jetons reconnus

| Caractère(s) | Jeton produit | Classe C++ |
|---|---|---|
| `(` | `OPENPAR` | `Symbole` |
| `)` | `CLOSEPAR` | `Symbole` |
| `+` | `PLUS` | `Symbole` |
| `*` | `MULT` | `Symbole` |
| `0`–`9` (suite) | `INT` (valeur entière) | `Entier` |
| fin de chaîne | `FIN` | `Symbole` |
| tout autre | `ERREUR` | `Symbole` |

Les **entiers multi-chiffres** sont entièrement lus en une passe (ex : `"123"` → `Entier(123)`). Les espaces **ne sont pas reconnus** et provoquent une erreur syntaxique.

### Fonctionnement du tampon

`Consulter()` ne relit le flux que si `tampon == nullptr`. `Avancer()` se contente de mettre `tampon = nullptr` sans allouer. Cela garantit que chaque jeton est créé exactement une fois et transféré à la pile de symboles (qui prend en charge sa suppression).

---

## 4. Hiérarchie des symboles

Tous les objets circulant dans les deux piles dérivent de `Symbole`.

```
Symbole          (ident : int)
├── Entier       (ident = INT,  valeur : int)   ← produit par le lexer
└── Expression   (ident = E,    valeur : int)   ← produit par une réduction
```

### Énumération des identifiants

```cpp
enum Identificateurs {
    OPENPAR,   // (
    CLOSEPAR,  // )
    PLUS,      // +
    MULT,      // *
    INT,       // entier littéral
    FIN,       // fin de chaîne ($)
    ERREUR,    // caractère non reconnu
    E,         // non-terminal : expression (utilisé pour GOTO)
    E_PRIME    // (réservé, non utilisé dans cette grammaire)
};
```

### Méthode `getValeur()`

- `Symbole::getValeur()` retourne `-1` (pas de valeur pour les opérateurs/délimiteurs).
- `Entier::getValeur()` retourne la valeur du littéral lu.
- `Expression::getValeur()` retourne la valeur calculée lors de la réduction.

C'est par cette méthode que les réductions récupèrent les valeurs partielles pour effectuer le calcul arithmétique.

---

## 5. Grammaire et théorie LALR(1)

### Grammaire source (ambiguë G1)

```
1)  E' → E
2)  E  → E + E
3)  E  → E * E
4)  E  → ( E )
5)  E  → val
```

La grammaire est **intentionnellement ambiguë** : les productions 2 et 3 ne précisent ni la priorité ni l'associativité. Ces propriétés sont exprimées directement par la **résolution manuelle des conflits** dans la table LALR(1) (voir section 6).

### Principe de l'analyse ascendante (bottom-up)

L'analyse LALR(1) lit l'entrée de gauche à droite et construit la dérivation **de droite à gauche** (réductions). À chaque étape :

- **Décalage (shift)** : on empile le symbole courant et on avance dans l'entrée.
- **Réduction (reduce)** : on remplace le sommet de la pile (correspondant à un membre droit d'une production) par le non-terminal du membre gauche.
- **Acceptation** : l'entrée est épuisée et la pile contient uniquement `E`.

L'automate LALR(1) sait quelle action effectuer grâce à deux informations :
1. L'**état courant** (sommet de `pilEtat`).
2. Le **symbole d'anticipation** (jeton retourné par `Consulter()`).

---

## 6. Table LALR(1) et résolution des conflits

### Table complète

| État | `val` | `+` | `*` | `(` | `)` | `$` | **E** (GOTO) |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 0 | d3 | | | d2 | | | **1** |
| 1 | | d4 | d5 | | | **acc** | |
| 2 | d3 | | | d2 | | | **6** |
| 3 | | r5 | r5 | | r5 | r5 | |
| 4 | d3 | | | d2 | | | **7** |
| 5 | d3 | | | d2 | | | **8** |
| 6 | | d4 | d5 | | d9 | | |
| 7 | | **r2** | **d5** | | r2 | r2 | |
| 8 | | **r3** | **r3** | | r3 | r3 | |
| 9 | | r4 | r4 | | r4 | r4 | |

`d` = décalage (shift), `r` = réduction (reduce), `acc` = acceptation, chiffre seul = GOTO.

Productions : `r2 = E→E+E` · `r3 = E→E*E` · `r4 = E→(E)` · `r5 = E→val`

### Les 4 conflits shift/reduce et leur résolution

La grammaire ambiguë génère 4 conflits. Chacun est résolu par une règle sémantique :

| État | Symbole | Conflit initial | Résolution choisie | Règle appliquée |
|:---:|:---:|---|:---:|---|
| **7** | `+` | d4 (shift `+`) ou r2 (réduire `E+E`) | **r2** | Associativité **gauche** de `+` : `1+2+3` = `(1+2)+3` |
| **7** | `*` | d5 (shift `*`) ou r2 (réduire `E+E`) | **d5** | Priorité de `*` **sur** `+` : `1+2*3` = `1+(2*3)` |
| **8** | `+` | d4 (shift `+`) ou r3 (réduire `E*E`) | **r3** | Priorité de `*` **sur** `+` : `2*3+4` = `(2*3)+4` |
| **8** | `*` | d5 (shift `*`) ou r3 (réduire `E*E`) | **r3** | Associativité **gauche** de `*` : `2*3*4` = `(2*3)*4` |

> **Pourquoi l'état 7 est le plus critique ?**
> C'est l'état atteint après avoir lu `E + E`. Si le prochain token est `*`, on doit
> **décaler** (d5) pour donner la priorité à `*`. Si c'est `+` ou `$`, on réduit (r2)
> pour respecter l'associativité gauche de `+`.

---

## 7. Design pattern State — les états

### Principe

Chaque ligne de la table LALR(1) est représentée par une classe C++ concrète héritant de la classe abstraite `Etat` :

```cpp
class Etat {
public:
    virtual void transition(Automate& a, Symbole* s) = 0;
    virtual ~Etat() {}
};
```

Les classes concrètes sont `Etat0` à `Etat9`. Chacune implémente `transition()` via un `switch` sur l'identifiant du symbole d'anticipation.

### Avantages du pattern State ici

- **Encapsulation** : chaque état porte sa propre logique de transition, sans table centrale à maintenir.
- **Extensibilité** : ajouter un état revient à créer une nouvelle classe sans toucher aux existantes.
- **Lisibilité** : le code de chaque état correspond directement à sa ligne dans la table LALR(1).

### Cycle de vie des objets `Etat`

Les états sont créés sur le tas (`new Etat3()`) et empilés dans `pilEtat`. Lors d'une réduction, chaque état dépilé se supprime lui-même (`delete this`) **immédiatement après le `pe.pop()`**, libérant la mémoire au fur et à mesure.

### Description de chaque état

| État | Situation dans l'analyse | Actions |
|:---:|---|---|
| **Etat0** | État initial | Shift `val`→Etat3, shift `(`→Etat2, GOTO E→Etat1 |
| **Etat1** | `E` reconnu au niveau racine | Shift `+`→Etat4, shift `*`→Etat5, `$`→acceptation |
| **Etat2** | Après `(` | Shift `val`→Etat3, shift `(`→Etat2 (récursif), GOTO E→Etat6 |
| **Etat3** | Après `val` | Reduce r5 (`E → val`) sur `+`, `*`, `)`, `$` |
| **Etat4** | Après `E +` | Shift `val`→Etat3, shift `(`→Etat2, GOTO E→Etat7 |
| **Etat5** | Après `E *` | Shift `val`→Etat3, shift `(`→Etat2, GOTO E→Etat8 |
| **Etat6** | Après `( E` | Shift `+`→Etat4, shift `*`→Etat5, shift `)`→Etat9 |
| **Etat7** | Après `E + E` | **Conflits résolus** : r2 sur `+`,`)`,`$` ; d5 sur `*` |
| **Etat8** | Après `E * E` | Reduce r3 (`E → E*E`) sur `+`, `*`, `)`, `$` |
| **Etat9** | Après `( E )` | Reduce r4 (`E → (E)`) sur `+`, `*`, `)`, `$` |

### Exemple de réduction r2 (`E → E + E`) dans Etat7

```cpp
// Dépiler les 3 symboles du membre droit
Symbole* e2 = ps.top(); ps.pop();   // E (droite)
Symbole* op = ps.top(); ps.pop();   // +
Symbole* e1 = ps.top(); ps.pop();   // E (gauche)
int v = e1->getValeur() + e2->getValeur();
delete e1; delete op; delete e2;

// Dépiler les 3 états correspondants
pe.pop(); delete this;              // Etat7
Etat* s4   = pe.top(); pe.pop(); delete s4;   // Etat4
Etat* prev = pe.top(); pe.pop(); delete prev; // état avant E

// Empiler le résultat et appeler GOTO
ps.push(new Expression(v));
pe.top()->transition(a, new Symbole(E));
```

---

## 8. L'automate — boucle principale

### Structure interne d'`Automate`

```cpp
class Automate {
    stack<Etat*>    pilEtat;     // pile des états courants
    stack<Symbole*> pilSymbole;  // pile des symboles et valeurs
    int   resultat;              // résultat final après acceptation
    bool  accepte;               // true si l'analyse a réussi
    bool  erreur;                // true si une erreur a été détectée
    Lexer* lexer;                // pointeur vers le lexer actif (non nul pendant Analyser)
};
```

### Méthode `Analyser(const string& expression)`

```
1. Réinitialisation complète des deux piles, des drapeaux et du résultat
2. Création d'un Lexer local sur la chaîne d'entrée
3. Empilement de l'Etat0 (état initial)
4. Boucle : tant que !accepte et !erreur
       symbole_courant = lexer.Consulter()
       pilEtat.top()->transition(this, symbole_courant)
5. Nettoyage mémoire (piles vidées, états et symboles supprimés)
6. Retour de accepte (true = succès, false = erreur syntaxique)
```

L'évaluation arithmétique est **entremêlée** à l'analyse : chaque réduction calcule immédiatement la valeur partielle et la stocke dans une `Expression` empilée. Le résultat final est donc disponible dès l'acceptation via `GetResultat()`.

### Gestion mémoire

- Chaque `Symbole*` alloué par le `Lexer` ou lors d'une réduction est supprimé soit dans la réduction elle-même, soit lors du nettoyage final.
- Chaque `Etat*` se supprime lui-même (`delete this`) après le `pe.pop()` dans une réduction, ou lors du nettoyage final.
- `Analyser()` peut être appelé **plusieurs fois** sur le même objet `Automate` : la réinitialisation en début de méthode remet toutes les structures à zéro.

---

## 9. Programmes d'entrée

### main.cpp

Programme interactif avec menu. Deux modes :

- **Tests automatiques** : analyse 10 expressions codées en dur et affiche le résultat avec `✓` ou `✗`.
- **Mode interactif** : boucle `getline` jusqu'à `q`. Chaque expression est analysée et évaluée à la volée.

### tester.cpp

Point d'entrée minimaliste pour les scripts de test automatisés :

```bash
./tester "expression"
```

- Retourne le résultat (entier) sur `stdout` si l'expression est valide.
- Retourne un message d'erreur sur `stderr` et quitte avec le code 1 si invalide.

---

## 10. Suite de tests

La suite complète est dans `tests.cpp`. Elle s'exécute avec `make test`.

### Organisation

| Suite | Description | Nb de tests |
|:---:|---|:---:|
| **[1]** | Valeurs simples (entiers seuls) | 8 |
| **[2]** | Addition — cas de base et neutres | 6 |
| **[3]** | Multiplication — cas de base et neutres | 6 |
| **[4]** | Priorité `*` avant `+` | 9 |
| **[5]** | Associativité gauche — chaînes longues | 9 |
| **[6]** | Parenthèses simples | 9 |
| **[7]** | Parenthèses imbriquées profondes (jusqu'à 7 niveaux) | 11 |
| **[8]** | Expressions complexes combinées | 11 |
| **[9]** | Conflits LALR(1) ciblés | 9 |
| **[10]** | Réutilisation du même objet `Automate` | 14 |
| **[11]** | Expressions invalides simples | 14 |
| **[12]** | Expressions invalides complexes | 17 |
| | **Total** | **123** |

### Détail des suites importantes

**Suite [4] — Priorité `*` avant `+`**

Ces tests vérifient directement la résolution du conflit en Etat7 sur `*` (shift d5 au lieu de reduce r2). Un parseur mal configuré donnerait `1+2*3 = 9` au lieu de `7`.

```
"1+2*3"   → 7     (1+(2*3), pas (1+2)*3=9)
"2*3+4"   → 10    ((2*3)+4, pas 2*(3+4)=14)
"1+2*3*4" → 25    (1+((2*3)*4))
```

**Suite [5] — Associativité gauche**

```
"1+2+3+4+5+6+7+8+9+10" → 55    (((((((((1+2)+3)+4)+5)+6)+7)+8)+9)+10)
"1*2*3*4*5*6"           → 720   (6! via associativité gauche de *)
```

**Suite [7] — Parenthèses imbriquées profondes**

Stress-test de la pile d'états : chaque `(` empile un Etat2, chaque `)` déclenche r4.

```
"(((((((42)))))))" → 42    (7 niveaux d'imbrication)
```

**Suite [8] — Expressions complexes**

```
"(1+2*3)*(4+5*6)"       → 238     (7 * 34)
"2*(3+4*(5+6))"         → 94      (2 * (3 + 44))
"(1+1)*(2+2)*(3+3)*(4+4)" → 384  (2*4*6*8)
"100*100*100"           → 1000000
"(100+200)*(100+200)"   → 90000
```

**Suite [9] — Conflits LALR(1) ciblés**

Chaque test vise un conflit précis de la table :

```
"1+2*3"  → 7    conflit Etat7/MULT  → shift (priorité *)
"2*3+4"  → 10   conflit Etat8/PLUS  → reduce (priorité *)
"2*3*4"  → 24   conflit Etat8/MULT  → reduce (assoc. gauche *)
"1+2+3"  → 6    conflit Etat7/PLUS  → reduce (assoc. gauche +)
```

**Suite [10] — Réutilisation de l'automate**

Valide que `Analyser()` réinitialise correctement l'état interne : alternance d'expressions valides et invalides sur un même objet, vérification que le résultat précédent ne "fuite" pas.

**Suites [11] et [12] — Expressions invalides**

Couvre les erreurs syntaxiques à tous les niveaux :

```
""            → rejeté   (vide)
"+1"          → rejeté   (opérateur en début)
"(1+2"        → rejeté   (parenthèse non fermée)
"1+2)"        → rejeté   (parenthèse en trop)
"((1+2)+(3+4)" → rejeté  (profond : parenthèse non fermée)
"(1+2)*()"    → rejeté   (sous-expression vide)
"1+(+2)"      → rejeté   (opérateur en début dans sous-expression)
```

---

## 11. Bilan et résultats

### Résultats des tests

```
==============================================
  Resultats : 123/123 passes
  Tous les tests sont PASSES !
==============================================
```

### Ce qui fonctionne

- **Analyse syntaxique complète** : toutes les expressions arithmétiques formées de `+`, `*`, parenthèses et entiers sont reconnues correctement.
- **Priorité des opérateurs** : `*` a bien priorité sur `+`, vérifiée par les tests ciblés de la suite [4] et [9].
- **Associativité gauche** : `1+2+3` = `(1+2)+3` et `2*3*4` = `(2*3)*4`, vérifiée jusqu'à 10 opérandes (suite [5]).
- **Parenthèses imbriquées** : jusqu'à 7 niveaux testés sans problème de pile.
- **Évaluation arithmétique** : le résultat est calculé lors des réductions, pas dans une passe séparée.
- **Rejet des expressions invalides** : 31 cas d'erreur couverts, avec détection précise de l'état en erreur affiché sur `stderr`.
- **Gestion mémoire** : pas de fuite mémoire — chaque `Etat*` et `Symbole*` alloué est supprimé.
- **Réutilisation de l'automate** : `Analyser()` est ré-entrant sur le même objet, sans état résiduel.

### Choix de conception

| Choix | Justification |
|---|---|
| Grammaire ambiguë + résolution manuelle des conflits | Permet d'exprimer simplement la priorité et l'associativité sans alourdir la grammaire avec des niveaux de précédence |
| Design pattern State | Chaque état encapsule sa propre logique ; le code suit directement la table LALR(1) |
| `delete this` dans les réductions | Libération mémoire immédiate et localisée, sans passe de nettoyage séparée |
| Évaluation pendant l'analyse | Pas besoin d'un AST intermédiaire ; la valeur est propagée via `getValeur()` sur la pile |
| Lexer avec tampon d'un seul jeton | Suffit pour un look-ahead de 1 (LALR(1)) ; simple et sans sur-consommation |

### Limites

- **Pas de soustraction ni de division** : la grammaire ne définit que `+` et `*`.
- **Entiers uniquement** : pas de flottants, pas de négatifs (un `-` serait une erreur lexicale).
- **Pas d'espaces** : le lexer ne les ignore pas — un espace dans l'expression provoque une erreur.
- **Débordement entier** : aucune vérification sur les grands résultats (overflow C++ `int`).
