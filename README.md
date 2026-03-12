# TP Compilation — Analyseur syntaxique LALR(1)

Analyseur syntaxique ascendant avec évaluation d'expressions arithmétiques,
implémenté via l'automate LALR(1) et le **design pattern State**.

---

## Compilation

```bash
make          # compile → produit l'exécutable "analyseur"
make run      # compile puis exécute
make clean    # supprime les fichiers objets et l'exécutable
```

---

## Utilisation

```bash
./analyseur
```

Au lancement, un menu propose deux options :

**1 — Tests automatiques** : exécute les 10 expressions prédéfinies et affiche les résultats.

**2 — Mode interactif** : entrez vos propres expressions une par une. Tapez `q` pour quitter.

```
  > 1+2*3
  ✓ Résultat: 7
  > (5+3)*2
  ✓ Résultat: 16
  > q
```

**Mode ligne de commande** (tests de l'équipe enseignante) :
```bash
g++ -Wall -std=c++11 -o tester tester.cpp symbole.o lexer.o etat.o automate.o
./tester "1+2*3"
# → 7
```

---

## Structure du projet

| Fichier | Rôle |
|---|---|
| `symbole.h/cpp` | Hiérarchie des symboles : `Symbole`, `Entier`, `Expression` |
| `lexer.h/cpp` | Analyseur lexical — tokenisation de la chaîne d'entrée |
| `etat.h/cpp` | **Design pattern State** — classes `Etat0` … `Etat9` |
| `automate.h/cpp` | Automate LALR(1) — boucle d'analyse + évaluation |
| `main.cpp` | Programme principal : menu interactif + tests automatiques |
| `tester.cpp` | Point d'entrée pour les tests de l'équipe enseignante |
| `Makefile` | Règles de compilation |

---

## Architecture — Automate LALR(1) + Pattern State

### Grammaire (ambiguë G1)

```
1)  E' → E
2)  E  → E + E   (associativité gauche)
3)  E  → E * E   (associativité gauche, priorité sur +)
4)  E  → ( E )
5)  E  → val
```

### Structures de données

- `stack<Etat*> pilEtat` — pile des états courants
- `stack<Symbole*> pilSymbole` — pile des symboles et valeurs intermédiaires

### Boucle principale (`Automate::Analyser`)

```
tant que non accepté et non erreur :
    pilEtat.top()->transition(automate, lexer.Consulter())
```

Chaque état concret implémente `transition()` selon la table LALR(1) :

| Action | Comportement |
|---|---|
| Décalage `dN` | Empile le symbole, avance le lexer, empile `EtatN` |
| Réduction `rX` | Dépile \|rhs\| symboles + états, calcule la valeur, appelle GOTO |
| Acceptation | Enregistre le résultat, arrête la boucle |
| GOTO (`E` reçu) | Empile l'état de destination, ne touche pas au lexer |

### Table LALR(1) résolue

| État | val | `+` | `*` | `(` | `)` | `$` | E |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 0 | d3 | | | d2 | | | 1 |
| 1 | | d4 | d5 | | | **acc** | |
| 2 | d3 | | | d2 | | | 6 |
| 3 | | r5 | r5 | | r5 | r5 | |
| 4 | d3 | | | d2 | | | 7 |
| 5 | d3 | | | d2 | | | 8 |
| 6 | | d4 | d5 | | d9 | | |
| 7 | | r2 | **d5** | | r2 | r2 | |
| 8 | | r3 | r3 | | r3 | r3 | |
| 9 | | r4 | r4 | | r4 | r4 | |

`r2 = E→E+E` &nbsp; `r3 = E→E*E` &nbsp; `r4 = E→(E)` &nbsp; `r5 = E→val`

### Résolution des conflits

| État | Symbole | Conflit | Résolution | Raison |
|:---:|:---:|:---:|:---:|---|
| 7 | `+` | d4 / r2 | **r2** | associativité gauche de `+` |
| 7 | `*` | d5 / r2 | **d5** | priorité de `*` sur `+` |
| 8 | `+` | d4 / r3 | **r3** | priorité de `*` sur `+` |
| 8 | `*` | d5 / r3 | **r3** | associativité gauche de `*` |

---

## Exemples de test

| Expression | Résultat | Note |
|---|:---:|---|
| `42` | 42 | |
| `1+2` | 3 | |
| `2*3` | 6 | |
| `1+2*3` | **7** | priorité : `1+(2*3)` |
| `2*3+4` | 10 | `(2*3)+4` |
| `(1+2)*3` | 9 | |
| `3*(2+4)` | 18 | |
| `10+20+30` | 60 | associativité gauche |
| `(5+5)*(2+8)` | 100 | |
| `((3+4)*5)` | 35 | parenthèses imbriquées |

---

## Notes

- Compilation sans warnings : `g++ -Wall -std=c++11`
- Gestion mémoire : chaque symbole est supprimé après réduction ; les états se suppriment eux-mêmes après dépilage (`delete this`)
- L'évaluation est réalisée à l'intérieur du programme d'analyse (lors de chaque réduction), conformément à l'énoncé
