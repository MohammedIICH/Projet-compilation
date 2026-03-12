# TP Compilation — Analyseur syntaxique LALR(1)

Analyseur syntaxique ascendant avec évaluation d'expressions arithmétiques,
implémenté en C++11 via l'automate LALR(1) et le **design pattern State**.

---

## Table des matières

1. [Vue d'ensemble](#1-vue-densemble)
2. [Compilation et utilisation](#2-compilation-et-utilisation)
3. [Structure des fichiers](#3-structure-des-fichiers)
4. [Analyse lexicale — Lexer](#4-analyse-lexicale--lexer)
5. [Hiérarchie des symboles](#5-hiérarchie-des-symboles)
6. [Grammaire et théorie LALR(1)](#6-grammaire-et-théorie-lalr1)
7. [Table LALR(1) et résolution des conflits](#7-table-lalr1-et-résolution-des-conflits)
8. [Design pattern State — les états](#8-design-pattern-state--les-états)
9. [L'automate — boucle principale](#9-lautomate--boucle-principale)
10. [Trace d'exécution pas à pas](#10-trace-dexécution-pas-à-pas)
11. [Programmes d'entrée](#11-programmes-dentrée)
12. [Suite de tests](#12-suite-de-tests)
13. [Bilan et résultats](#13-bilan-et-résultats)

---

## 1. Vue d'ensemble

Ce projet implémente un **analyseur syntaxique ascendant LALR(1)** capable d'analyser et d'évaluer des expressions arithmétiques impliquant l'addition (`+`), la multiplication (`*`) et les parenthèses.

### Ce que le programme sait faire

- Reconnaître si une expression est **syntaxiquement valide**
- **Évaluer** le résultat arithmétique en respectant :
  - la **priorité** de `*` sur `+`
  - l'**associativité gauche** de `+` et de `*`
  - le **parenthésage** arbitraire et imbriqué
- **Rejeter** toute expression malformée avec un message d'erreur indiquant l'état en défaut

### Exemples

```
"1+2*3"                    →  7       (pas 9 : * est prioritaire)
"(1+2)*3"                  →  9       (parenthèses inversent la priorité)
"1+2+3+4+5+6+7+8+9+10"    →  55      (associativité gauche sur 10 opérandes)
"(1+2*3)*(4+5*6)"          →  238     (7 × 34)
"100*100*100"              →  1000000
"+1"                       →  erreur syntaxique
"(1+2"                     →  erreur syntaxique
```

---

## 2. Compilation et utilisation

### Prérequis

- `g++` avec support C++11 (`-std=c++11`)
- `make` (optionnel, toutes les commandes équivalentes sont données)

### Commandes make

```bash
make           # compile → produit l'exécutable "analyseur"
make test      # compile et exécute la suite de tests complète (tests.exe)
make clean     # supprime tous les fichiers objets et exécutables générés
```

### Compilation manuelle (sans make)

```bash
# Étape 1 : compiler les fichiers objets partagés
g++ -Wall -std=c++11 -c symbole.cpp
g++ -Wall -std=c++11 -c lexer.cpp
g++ -Wall -std=c++11 -c etat.cpp
g++ -Wall -std=c++11 -c automate.cpp

# Étape 2a : programme interactif (analyseur)
g++ -Wall -std=c++11 -c main.cpp
g++ -Wall -std=c++11 -o analyseur main.o symbole.o lexer.o etat.o automate.o

# Étape 2b : suite de tests (tests.exe)
g++ -Wall -std=c++11 -c tests.cpp
g++ -Wall -std=c++11 -o tests.exe tests.o symbole.o lexer.o etat.o automate.o

# Étape 2c : calculatrice en ligne de commande (tester.exe)
g++ -Wall -std=c++11 -c tester.cpp
g++ -Wall -std=c++11 -o tester.exe tester.o symbole.o lexer.o etat.o automate.o
```

### Lancer le programme principal (`analyseur`)

```bash
./analyseur
```

Un menu s'affiche au démarrage :

```
╔════════════════════════════════════════════════════════════╗
║      Analyseur Syntaxique LALR(1) & Évaluateur            ║
║     Grammaire avec Priorité et Associativité              ║
╚════════════════════════════════════════════════════════════╝

Grammaire:
  E' → E
  E  → E + E  (associativité gauche)
  E  → E * E  (associativité gauche, priorité > +)
  E  → ( E )
  E  → val

Que voulez-vous faire ?
  1. Lancer les tests automatiques
  2. Entrer une expression manuellement
  >
```

**Option 1 — Tests automatiques** : analyse 10 expressions prédéfinies et affiche leurs résultats.

**Option 2 — Mode interactif** : entrez vos propres expressions une par une (tapez `q` pour quitter).

```
  > 1+2*3
  ✓ Résultat: 7
  > (5+3)*2
  ✓ Résultat: 16
  > +1
  ✗ Erreur syntaxique
  > q
```

### Lancer la suite de tests (`tests.exe`)

```bash
./tests.exe
```

Résultat attendu :

```
==============================================
  Resultats : 123/123 passes
  Tous les tests sont PASSES !
==============================================
```

### Utiliser la calculatrice CLI (`tester.exe`)

```bash
./tester.exe "1+2*3"       # affiche : 7
./tester.exe "(10+20)*3"   # affiche : 90
./tester.exe "abc"         # stderr : Erreur de syntaxe, exit code 1
```

---

## 3. Structure des fichiers

```
Projet-compilation/
├── Makefile          — règles de compilation (all, test, clean)
├── symbole.h/cpp     — hiérarchie des symboles (Symbole, Entier, Expression)
├── lexer.h/cpp       — analyseur lexical (tokenisation de la chaîne d'entrée)
├── etat.h/cpp        — design pattern State : Etat0 … Etat9
├── automate.h/cpp    — automate LALR(1), boucle principale, gestion mémoire
├── main.cpp          — programme interactif avec menu
├── tester.cpp        — calculatrice CLI (expression passée en argument)
└── tests.cpp         — suite de 123 tests automatisés en 12 catégories
```

### Rôle de chaque fichier

| Fichier | Responsabilité |
|---|---|
| `symbole.h/cpp` | Définit les terminaux (`Entier`) et non-terminaux (`Expression`) manipulés par les piles |
| `lexer.h/cpp` | Découpe la chaîne d'entrée en jetons (tokens) un par un avec look-ahead d'un symbole |
| `etat.h/cpp` | Encode toute la table LALR(1) via le pattern State (10 classes concrètes) |
| `automate.h/cpp` | Gère les deux piles, pilote la boucle d'analyse, stocke le résultat final |
| `main.cpp` | Interface utilisateur : menu, mode interactif, 10 tests visuels prédéfinis |
| `tester.cpp` | Interface CLI minimaliste : prend l'expression en argument, retourne le résultat ou une erreur |
| `tests.cpp` | Suite de 123 tests organisés en 12 catégories, compteur de réussite, code de retour |

---

## 4. Analyse lexicale — Lexer

Le `Lexer` reçoit la chaîne d'entrée complète et la parcourt **caractère par caractère** en maintenant un **tampon d'un seul jeton** (look-ahead de 1 symbole).

### Interface publique

```cpp
Symbole* Consulter();  // retourne le jeton courant sans avancer dans le flux
void     Avancer();    // invalide le tampon → le prochain Consulter() lit le jeton suivant
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

### Entiers multi-chiffres

Les suites de chiffres consécutifs sont entièrement lues en une seule passe :

```
"123" → Entier(123)   (pas trois Entier séparés)
```

Le code accumule les chiffres tant que le caractère suivant est aussi un chiffre :

```cpp
int val = flux[tete] - '0';
int i   = 1;
while (tete + i < flux.length() && flux[tete + i] >= '0' && flux[tete + i] <= '9') {
    val = val * 10 + (flux[tete + i] - '0');
    i++;
}
tete += i;
tampon = new Entier(val);
```

### Fonctionnement du tampon (`tampon`)

- `Consulter()` ne relit le flux **que si `tampon == nullptr`**. Si le tampon est déjà rempli, il retourne immédiatement le même pointeur.
- `Avancer()` met simplement `tampon = nullptr` — il ne libère pas l'objet pointé, car la **propriété** du symbole est transférée à la pile de symboles lors d'un décalage (shift). En cas d'erreur ou si le symbole n'est pas consommé, la mémoire est gérée par le nettoyage final dans `Analyser()`.

### Espaces et caractères non reconnus

Le lexer **ne saute pas les espaces**. Un espace dans une expression provoque la production d'un `Symbole(ERREUR)`, ce qui entraîne immédiatement une erreur syntaxique dans l'automate.

---

## 5. Hiérarchie des symboles

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
    INT,       // entier littéral (terminal)
    FIN,       // fin de chaîne ($)
    ERREUR,    // caractère non reconnu
    E,         // non-terminal : expression réduite (utilisé pour les GOTO)
    E_PRIME    // réservé (non utilisé dans cette grammaire)
};
```

### `operator int()` et `getIdent()`

`Symbole` surcharge `operator int()` pour permettre d'utiliser un `Symbole*` directement dans un `switch` :

```cpp
switch (s->getIdent()) {  // getIdent() retourne le champ ident
    case INT:  ...
    case PLUS: ...
}
```

### Méthode virtuelle `getValeur()`

| Classe | Valeur retournée |
|---|---|
| `Symbole` | `-1` (pas de valeur pour les opérateurs/délimiteurs) |
| `Entier` | La valeur du littéral lu (`42` pour `"42"`) |
| `Expression` | La valeur calculée lors de la réduction |

C'est par `getValeur()` que les réductions **récupèrent les valeurs partielles** et effectuent le calcul arithmétique sans avoir à transtyper les pointeurs.

---

## 6. Grammaire et théorie LALR(1)

### Grammaire source (ambiguë G1)

```
1)  E' → E
2)  E  → E + E
3)  E  → E * E
4)  E  → ( E )
5)  E  → val
```

La grammaire est **intentionnellement ambiguë** : les productions 2 et 3 ne précisent ni la priorité ni l'associativité. Ces propriétés sont exprimées par la **résolution manuelle des conflits** dans la table LALR(1) (voir section 7).

### Principe de l'analyse ascendante (bottom-up)

L'analyse LALR(1) lit l'entrée de gauche à droite et construit la dérivation **de droite à gauche** (réductions). Les deux piles évoluent en parallèle :

- **`pilSymbole`** : contient les terminaux et non-terminaux reconnus jusqu'ici
- **`pilEtat`** : contient les états LALR(1) correspondant à chaque symbole empilé

À chaque étape, l'automate consulte le **symbole d'anticipation** (jeton courant du lexer) et l'**état au sommet de `pilEtat`** pour décider de l'action :

| Action | Description |
|---|---|
| **Décalage (shift dN)** | Empile le symbole courant, avance le lexer, empile l'état N |
| **Réduction (reduce rX)** | Dépile `\|rhs\|` symboles et autant d'états, calcule la valeur, empile une `Expression`, appelle GOTO |
| **GOTO** | Après une réduction, l'état au nouveau sommet reçoit le non-terminal `E` et empile l'état cible |
| **Acceptation** | L'entrée est épuisée (`FIN`) et `E` couvre toute la pile → succès |
| **Erreur** | Aucune entrée valide dans la table pour l'état courant + symbole d'anticipation |

### Pourquoi LALR(1) et pas LR(0) ou SLR ?

La grammaire G1 est ambiguë, ce qui crée des **conflits shift/reduce**. LR(0) et SLR ne permettent pas de résoudre ces conflits sans modifier la grammaire. LALR(1) utilise un **look-ahead d'un symbole** pour décider quelle action appliquer, ce qui permet de résoudre les 4 conflits manuellement en imposant priorité et associativité.

---

## 7. Table LALR(1) et résolution des conflits

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

**Légende** : `dN` = décalage vers l'état N, `rX` = réduction avec la production X, `acc` = acceptation, chiffre seul = GOTO.

**Productions** : `r2 = E→E+E` · `r3 = E→E*E` · `r4 = E→(E)` · `r5 = E→val`

### Les 4 conflits shift/reduce et leur résolution

La grammaire ambiguë génère 4 conflits. Chacun est résolu manuellement :

| État | Symbole | Conflit initial | Résolution | Règle appliquée |
|:---:|:---:|---|:---:|---|
| **7** | `+` | shift d4 ou reduce r2 | **r2** | Associativité **gauche** de `+` : `1+2+3` = `(1+2)+3` |
| **7** | `*` | shift d5 ou reduce r2 | **d5** | Priorité de `*` **sur** `+` : `1+2*3` = `1+(2*3)` |
| **8** | `+` | shift d4 ou reduce r3 | **r3** | Priorité de `*` **sur** `+` : `2*3+4` = `(2*3)+4` |
| **8** | `*` | shift d5 ou reduce r3 | **r3** | Associativité **gauche** de `*` : `2*3*4` = `(2*3)*4` |

### Pourquoi l'état 7 est le plus critique

L'état 7 est atteint après avoir lu la forme sentencielle `E + E`. Le symbole d'anticipation détermine tout :

- Si c'est `*` → **shift d5** : on continue à lire le membre droit pour donner priorité à `*`.
- Si c'est `+`, `)` ou `$` → **reduce r2** : on réduit `E + E` en `E` avec associativité gauche.

Sans cette résolution, `"1+2*3"` serait analysé comme `(1+2)*3 = 9` au lieu de `1+(2*3) = 7`.

---

## 8. Design pattern State — les états

### Principe

Chaque ligne de la table LALR(1) est représentée par une **classe C++ concrète** héritant de la classe abstraite `Etat` :

```cpp
class Etat {
public:
    virtual void transition(Automate& a, Symbole* s) = 0;
    virtual ~Etat() {}
};
```

Les classes concrètes `Etat0` à `Etat9` implémentent `transition()` avec un `switch` sur `s->getIdent()`. Chacune encode exactement sa ligne dans la table LALR(1).

### Avantages de ce pattern

| Avantage | Explication |
|---|---|
| **Encapsulation** | Chaque état porte sa propre logique de transition, sans table centrale à maintenir |
| **Correspondance directe** | Le code de chaque état correspond ligne pour ligne à la table LALR(1) |
| **Extensibilité** | Ajouter un état = créer une nouvelle classe, sans modifier les existantes |
| **Localisation des erreurs** | Chaque état affiche son numéro sur `stderr` lors d'un défaut |

### Cycle de vie des objets `Etat*`

Les états sont créés sur le tas (`new EtatN()`) et **empilés dans `pilEtat`**. Lors d'une réduction, les états correspondant aux symboles du membre droit sont dépilés et **se suppriment eux-mêmes** avec `delete this` immédiatement après le `pop()` :

```cpp
pe.pop(); delete this;   // l'état courant se supprime après s'être dépilé
```

> **Important** : après `delete this`, on ne peut plus accéder à aucun membre de l'objet. C'est pourquoi les références locales `pe` et `ps` (pointant vers l'`Automate`, toujours vivant) sont utilisées pour toutes les opérations suivantes.

### Description de chaque état

| État | Situation dans l'analyse | Actions |
|:---:|---|---|
| **Etat0** | État initial | Shift `val`→Etat3, shift `(`→Etat2, GOTO `E`→Etat1 |
| **Etat1** | `E` reconnu au niveau racine | Shift `+`→Etat4, shift `*`→Etat5, `$`→acceptation |
| **Etat2** | Après `(` | Shift `val`→Etat3, shift `(`→Etat2 (récursif), GOTO `E`→Etat6 |
| **Etat3** | Après `val` | Reduce r5 (`E→val`) sur `+`, `*`, `)`, `$` |
| **Etat4** | Après `E +` | Shift `val`→Etat3, shift `(`→Etat2, GOTO `E`→Etat7 |
| **Etat5** | Après `E *` | Shift `val`→Etat3, shift `(`→Etat2, GOTO `E`→Etat8 |
| **Etat6** | Après `( E` | Shift `+`→Etat4, shift `*`→Etat5, shift `)`→Etat9 |
| **Etat7** | Après `E + E` | **Conflits résolus** : r2 sur `+`,`)`,`$` ; shift d5 sur `*` |
| **Etat8** | Après `E * E` | Reduce r3 (`E→E*E`) sur `+`, `*`, `)`, `$` |
| **Etat9** | Après `( E )` | Reduce r4 (`E→(E)`) sur `+`, `*`, `)`, `$` |

### Exemple de réduction r2 (`E → E + E`) dans Etat7

```cpp
// --- Pile avant réduction : ... | E(g) | + | E(d) |   (sommet →)
//     Pile états :           ... | prev | Etat4 | Etat7 |

// 1. Dépiler les 3 symboles du membre droit
Symbole* e2 = ps.top(); ps.pop();   // E droite
Symbole* op = ps.top(); ps.pop();   // +
Symbole* e1 = ps.top(); ps.pop();   // E gauche
int v = e1->getValeur() + e2->getValeur();
delete e1; delete op; delete e2;

// 2. Dépiler les 3 états correspondants
pe.pop(); delete this;              // Etat7 (se supprime lui-même)
Etat* s4   = pe.top(); pe.pop(); delete s4;   // Etat4
Etat* prev = pe.top(); pe.pop(); delete prev; // état précédant E gauche

// 3. Empiler le résultat et déclencher le GOTO
ps.push(new Expression(v));
pe.top()->transition(a, new Symbole(E));  // l'état restant au sommet gère le GOTO
```

### Exemple de réduction r4 (`E → ( E )`) dans Etat9

```cpp
Symbole* close = ps.top(); ps.pop();  // )
Symbole* e     = ps.top(); ps.pop();  // E
Symbole* open  = ps.top(); ps.pop();  // (
int v = e->getValeur();
delete close; delete e; delete open;

pe.pop(); delete this;                // Etat9
Etat* s6 = pe.top(); pe.pop(); delete s6;  // Etat6
Etat* s2 = pe.top(); pe.pop(); delete s2;  // Etat2

ps.push(new Expression(v));
pe.top()->transition(a, new Symbole(E));
```

---

## 9. L'automate — boucle principale

### Structure interne d'`Automate`

```cpp
class Automate {
    stack<Etat*>    pilEtat;     // pile des états courants
    stack<Symbole*> pilSymbole;  // pile des symboles et valeurs intermédiaires
    int   resultat;              // résultat final après acceptation
    bool  accepte;               // true si l'analyse a réussi
    bool  erreur;                // true si une erreur a été détectée
    Lexer* lexer;                // pointeur vers le lexer actif (non nul pendant Analyser)
};
```

### Accesseurs pour le pattern State

Les états ont besoin de manipuler les piles et les drapeaux de l'automate. Ces accès passent par des méthodes publiques dédiées :

```cpp
stack<Etat*>&    getPilEtat()    // accès à la pile d'états
stack<Symbole*>& getPilSymbole() // accès à la pile de symboles
void setResultat(int r)          // stocke le résultat final
void setAccepte()                // signale l'acceptation
void setErreur()                 // signale une erreur syntaxique
void avancerLexer()              // appelle lexer->Avancer()
```

### Méthode `Analyser(const string& expression)`

```
1. Réinitialisation complète :
   - vider et supprimer les deux piles (états + symboles)
   - remettre accepte = false, erreur = false, resultat = 0

2. Créer un Lexer local sur la chaîne d'entrée

3. Empiler Etat0 (état initial)

4. Boucle principale :
   tant que !accepte && !erreur :
       sym = lexer->Consulter()               ← look-ahead (ne consomme pas)
       pilEtat.top()->transition(this, sym)   ← délégation au pattern State

5. Nettoyage mémoire :
   - vider et supprimer les deux piles (restes après erreur ou acceptation)

6. Retourner accepte
```

L'évaluation arithmétique est **entremêlée** à l'analyse : chaque réduction calcule immédiatement la valeur partielle et la stocke dans une `Expression` empilée. Le résultat final est donc disponible dès l'acceptation via `GetResultat()` — sans AST intermédiaire.

### Gestion mémoire

- Chaque `Symbole*` alloué par le lexer ou lors d'une réduction est supprimé **dans la réduction elle-même** (après extraction des valeurs) ou lors du nettoyage final.
- Chaque `Etat*` se supprime lui-même avec `delete this` lors d'une réduction, ou lors du nettoyage final.
- `Analyser()` peut être appelé **plusieurs fois** sur le même objet `Automate` : la réinitialisation en début de méthode remet toutes les structures à zéro, sans état résiduel.

---

## 10. Trace d'exécution pas à pas

Trace complète pour `"1+2*3"` (résultat attendu : `7`).

```
Entrée : 1 + 2 * 3 $

Étape  Pile d'états               Pile symboles              Look-ahead  Action
──────────────────────────────────────────────────────────────────────────────────
 1     [E0]                       []                         INT(1)      shift d3
 2     [E0, E3]                   [INT(1)]                   PLUS        reduce r5 : E→val
           dépile E3 + INT(1), empile Expr(1), GOTO depuis E0 → E1
 3     [E0, E1]                   [Expr(1)]                  PLUS        shift d4
 4     [E0, E1, E4]               [Expr(1), +]               INT(2)      shift d3
 5     [E0, E1, E4, E3]           [Expr(1), +, INT(2)]       MULT        reduce r5 : E→val
           dépile E3 + INT(2), empile Expr(2), GOTO depuis E4 → E7
 6     [E0, E1, E4, E7]           [Expr(1), +, Expr(2)]      MULT        shift d5  ← conflit résolu !
 7     [E0, E1, E4, E7, E5]       [Expr(1), +, Expr(2), *]   INT(3)      shift d3
 8     [E0, E1, E4, E7, E5, E3]   [Expr(1),+,Expr(2),*,INT(3)]  FIN     reduce r5 : E→val
           dépile E3 + INT(3), empile Expr(3), GOTO depuis E5 → E8
 9     [E0, E1, E4, E7, E8]       [Expr(1),+,Expr(2),*,Expr(3)]  FIN    reduce r3 : E→E*E
           v = 2 * 3 = 6
           dépile E8+E5+E7, dépile Expr(3)+*+Expr(2), empile Expr(6), GOTO depuis E1 → E7
10     [E0, E1, E7]               [Expr(1), +, Expr(6)]      FIN         reduce r2 : E→E+E
           v = 1 + 6 = 7
           dépile E7+E4+E1, dépile Expr(6)+plus+Expr(1), empile Expr(7), GOTO depuis E0 → E1
11     [E0, E1]                   [Expr(7)]                  FIN         ACCEPTATION → 7
```

**Observation clé à l'étape 6** : en état 7, le look-ahead est `*`. La table dit `d5` (shift) plutôt que `r2` (reduce) → la multiplication est traitée en priorité avant d'effectuer l'addition.

---

## 11. Programmes d'entrée

### `main.cpp` — programme interactif

Deux modes d'utilisation via un menu textuel :

- **Tests automatiques** : analyse 10 expressions codées en dur et affiche le résultat avec `✓` ou `✗`.
- **Mode interactif** : boucle `getline(cin, expr)` jusqu'à `q` ou fin d'entrée. Chaque expression est analysée et évaluée immédiatement.

### `tester.cpp` — calculatrice CLI

Point d'entrée minimaliste, conçu pour une utilisation en script ou par l'équipe enseignante :

```bash
./tester.exe "expression"
```

- **Succès** : affiche le résultat (entier) sur `stdout`, code de retour `0`.
- **Erreur** : affiche `"Erreur de syntaxe"` sur `stderr`, code de retour `1`.

Utilisation dans un script shell :

```bash
result=$(./tester.exe "1+2*3")
echo "Résultat : $result"   # Résultat : 7
```

### `tests.cpp` — suite de tests

Programme autonome avec compteur de réussite. Voir section 12 pour le détail.

---

## 12. Suite de tests

La suite complète est dans `tests.cpp`. Elle compile vers `tests.exe` et s'exécute avec `./tests.exe` (ou `make test`).

### Organisation des 123 tests en 12 catégories

| Suite | Description | Nb de tests |
|:---:|---|:---:|
| **[1]** | Valeurs simples (entiers seuls, de 0 à 9999) | 8 |
| **[2]** | Addition — cas de base, cas neutres | 6 |
| **[3]** | Multiplication — cas de base, cas neutres | 6 |
| **[4]** | Priorité `*` avant `+` | 9 |
| **[5]** | Associativité gauche — chaînes d'opérateurs | 9 |
| **[6]** | Parenthèses simples | 9 |
| **[7]** | Parenthèses imbriquées profondes (jusqu'à 7 niveaux) | 11 |
| **[8]** | Expressions complexes combinées | 11 |
| **[9]** | Conflits LALR(1) ciblés | 9 |
| **[10]** | Réutilisation du même objet `Automate` | 14 |
| **[11]** | Expressions invalides simples | 14 |
| **[12]** | Expressions invalides complexes | 17 |
| | **Total** | **123** |

### Helpers de test

```cpp
// Test d'une expression valide : vérifie ok==true && résultat==attendu
testValide("1+2*3", 7);

// Test d'une expression invalide : vérifie ok==false
testInvalide("+1", "opérateur en début");

// Test de réutilisation (même objet Automate partagé entre appels)
testReutilisation(a, "1+2", 3);
testReutilisationInvalide(a, "+1");
```

Chaque test affiche `[OK]` ou `[FAIL]` avec les valeurs obtenues vs attendues.

### Détail des suites importantes

**Suite [4] — Priorité `*` avant `+`**

Ces tests vérifient directement la résolution du conflit en Etat7 (`*` → shift d5 plutôt que reduce r2). Un parseur mal configuré donnerait `9` au lieu de `7` pour le premier test :

```
"1+2*3"   → 7     (1+(2*3), PAS (1+2)*3 = 9)
"2*3+4"   → 10    ((2*3)+4, PAS 2*(3+4) = 14)
"1+2*3*4" → 25    (1+((2*3)*4) = 1+24)
```

**Suite [5] — Associativité gauche**

```
"1+2+3+4+5+6+7+8+9+10" → 55     (((((((((1+2)+3)+4)+5)+6)+7)+8)+9)+10)
"1*2*3*4*5*6"           → 720    (6! via associativité gauche de *)
"2*3+4*5+6*7"           → 68     (6+20+42)
```

**Suite [7] — Parenthèses imbriquées profondes**

Stress-test de la pile d'états : chaque `(` empile un Etat2 supplémentaire.

```
"(((((((42)))))))" → 42    (7 niveaux d'imbrication)
```

**Suite [8] — Expressions complexes**

```
"(1+2*3)*(4+5*6)"          → 238      (7 × 34)
"2*(3+4*(5+6))"            → 94       (2 × (3 + 44))
"(1+1)*(2+2)*(3+3)*(4+4)"  → 384     (2×4×6×8)
"(100+200)*(100+200)"      → 90000
"100*100*100"              → 1000000
```

**Suite [9] — Conflits LALR(1) ciblés**

Chaque test vise précisément un conflit de la table :

```
"1+2*3"  → 7    Etat7/MULT  : shift d5  (priorité * > +)
"2*3+4"  → 10   Etat8/PLUS  : reduce r3 (priorité * > +)
"2*3*4"  → 24   Etat8/MULT  : reduce r3 (associativité gauche de *)
"1+2+3"  → 6    Etat7/PLUS  : reduce r2 (associativité gauche de +)
```

**Suite [10] — Réutilisation de l'automate**

Valide que `Analyser()` réinitialise correctement l'état interne sur un même objet. Alternance d'expressions valides et invalides :

```cpp
Automate a;
testReutilisation(a,         "1+2",    3);
testReutilisationInvalide(a, "+1");         // erreur → reset correct
testReutilisation(a,         "3*4",   12);
testReutilisationInvalide(a, "1+");         // erreur → reset correct
testReutilisation(a,         "(1+2)*3", 9);
```

**Suites [11] et [12] — Expressions invalides**

Couvrent toutes les formes d'erreurs syntaxiques :

```
""               → rejeté  (vide)
"+1"             → rejeté  (opérateur en début)
"1+"             → rejeté  (opérateur en fin)
"()"             → rejeté  (parenthèses vides)
"(1+2"           → rejeté  (parenthèse non fermée)
"1+2)"           → rejeté  (parenthèse en trop)
"1++2"           → rejeté  (double opérateur)
"abc"            → rejeté  (caractères invalides)
"1 2"            → rejeté  (espace sans opérateur)
"((1+2)+(3+4)"   → rejeté  (profond : parenthèse non fermée)
"(1+2)*()"       → rejeté  (sous-expression vide)
"1+(+2)"         → rejeté  (opérateur en début dans sous-expression)
")*1"            → rejeté  (parenthèse fermante en début)
```

---

## 13. Bilan et résultats

### Résultats des tests

```
==============================================
  Resultats : 123/123 passes
  Tous les tests sont PASSES !
==============================================
```

### Ce qui fonctionne

| Fonctionnalité | Détail |
|---|---|
| **Analyse syntaxique** | Toutes les expressions valides sont reconnues et évaluées correctement |
| **Priorité des opérateurs** | `*` a bien priorité sur `+`, vérifiée par les suites [4] et [9] |
| **Associativité gauche** | `+` et `*` sont associatifs à gauche, vérifiés jusqu'à 10 opérandes (suite [5]) |
| **Parenthèses imbriquées** | Jusqu'à 7 niveaux testés sans problème de pile (suite [7]) |
| **Évaluation arithmétique** | Le résultat est calculé lors des réductions, sans AST intermédiaire |
| **Rejet des invalides** | 31 cas d'erreur couverts, avec détection précise de l'état en défaut |
| **Gestion mémoire** | Pas de fuite mémoire — chaque `Etat*` et `Symbole*` alloué est supprimé |
| **Réutilisation** | `Analyser()` est ré-entrant sur le même objet, sans état résiduel |

### Choix de conception

| Choix | Justification |
|---|---|
| **Grammaire ambiguë + résolution manuelle** | Permet d'exprimer simplement priorité et associativité sans alourdir la grammaire avec des niveaux de précédence supplémentaires |
| **Design pattern State** | Chaque état encapsule sa propre logique ; le code suit directement la table LALR(1), sans table de dispatch centrale |
| **`delete this` dans les réductions** | Libération mémoire immédiate et localisée : l'état sait exactement quand il ne sera plus jamais utilisé |
| **Évaluation pendant l'analyse** | Pas besoin d'un AST intermédiaire ; la valeur est propagée via `getValeur()` sur la pile de symboles |
| **Tampon d'un seul jeton dans le Lexer** | Suffit pour un look-ahead de 1 (LALR(1)) ; simple et sans sur-consommation de l'entrée |
| **Deux piles distinctes** | Sépare clairement la logique de contrôle (états) des données (symboles et valeurs) |

### Limites connues

| Limite | Description |
|---|---|
| **Opérateurs limités** | Seuls `+` et `*` sont supportés. Pas de soustraction (`-`), division (`/`), ni modulo (`%`) |
| **Entiers positifs uniquement** | Pas de flottants, pas de négatifs (un `-` est une erreur lexicale) |
| **Pas d'espaces** | Le lexer ne les ignore pas — un espace dans l'expression provoque une erreur syntaxique |
| **Débordement entier** | Aucune vérification sur les grands résultats (overflow C++ `int` non détecté) |
| **Pas d'AST** | L'évaluation directe pendant l'analyse ne permet pas de réutiliser la structure syntaxique pour d'autres traitements (optimisation, affichage d'arbre…) |
