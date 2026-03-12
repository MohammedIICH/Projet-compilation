#include "automate.h"
#include <iostream>
#include <string>

using namespace std;

// ─── Compteurs globaux ───────────────────────────────────────────────────────

static int passed = 0;
static int failed = 0;

// ─── Helpers ────────────────────────────────────────────────────────────────

static void testValide(const string& expr, int attendu) {
    Automate a;
    bool ok  = a.Analyser(expr);
    int  res = a.GetResultat();
    if (ok && res == attendu) {
        cout << "  [OK]   \"" << expr << "\" = " << res << "\n";
        ++passed;
    } else {
        cout << "  [FAIL] \"" << expr << "\""
             << "  attendu=" << attendu
             << "  obtenu=" << (ok ? to_string(res) : "ERREUR") << "\n";
        ++failed;
    }
}

static void testInvalide(const string& expr, const string& raison = "") {
    Automate a;
    bool ok = a.Analyser(expr);
    if (!ok) {
        cout << "  [OK]   \"" << expr << "\" => rejetee"
             << (raison.empty() ? "" : "  (" + raison + ")") << "\n";
        ++passed;
    } else {
        cout << "  [FAIL] \"" << expr << "\""
             << "  attendu=ERREUR  obtenu=" << a.GetResultat()
             << (raison.empty() ? "" : "  (" + raison + ")") << "\n";
        ++failed;
    }
}

// Teste la réutilisation d'un même objet Automate (réinitialisation correcte)
static void testReutilisation(Automate& a, const string& expr, int attendu) {
    bool ok  = a.Analyser(expr);
    int  res = a.GetResultat();
    if (ok && res == attendu) {
        cout << "  [OK]   [reuse] \"" << expr << "\" = " << res << "\n";
        ++passed;
    } else {
        cout << "  [FAIL] [reuse] \"" << expr << "\""
             << "  attendu=" << attendu
             << "  obtenu=" << (ok ? to_string(res) : "ERREUR") << "\n";
        ++failed;
    }
}

static void testReutilisationInvalide(Automate& a, const string& expr) {
    bool ok = a.Analyser(expr);
    if (!ok) {
        cout << "  [OK]   [reuse] \"" << expr << "\" => rejetee\n";
        ++passed;
    } else {
        cout << "  [FAIL] [reuse] \"" << expr << "\""
             << "  attendu=ERREUR  obtenu=" << a.GetResultat() << "\n";
        ++failed;
    }
}

// ─── Suite 1 : valeurs simples ───────────────────────────────────────────────

void tests_valeurs_simples() {
    cout << "\n=== [1] Valeurs simples ===\n";
    testValide("0",    0);
    testValide("1",    1);
    testValide("9",    9);
    testValide("42",   42);
    testValide("100",  100);
    testValide("999",  999);
    testValide("1000", 1000);
    testValide("9999", 9999);
}

// ─── Suite 2 : addition ─────────────────────────────────────────────────────

void tests_addition() {
    cout << "\n=== [2] Addition ===\n";
    testValide("1+2",   3);
    testValide("0+0",   0);
    testValide("5+0",   5);
    testValide("0+5",   5);
    testValide("10+20", 30);
    testValide("99+1",  100);
}

// ─── Suite 3 : multiplication ────────────────────────────────────────────────

void tests_multiplication() {
    cout << "\n=== [3] Multiplication ===\n";
    testValide("2*3",   6);
    testValide("0*5",   0);
    testValide("5*0",   0);
    testValide("1*42",  42);
    testValide("10*10", 100);
    testValide("7*8",   56);
}

// ─── Suite 4 : priorité opérateurs (* avant +) ──────────────────────────────
// Ces tests vérifient spécifiquement la résolution du conflit LALR(1)
// en état 7, symbole * : shift (d5) plutôt que reduce (r2).

void tests_priorite() {
    cout << "\n=== [4] Priorite (* avant +) ===\n";
    testValide("1+2*3",   7);    // 1+(2*3)=7, PAS (1+2)*3=9
    testValide("2*3+4",   10);   // (2*3)+4=10, PAS 2*(3+4)=14
    testValide("1*2+3*4", 14);   // 2+12=14
    testValide("1+2*3*4", 25);   // 1+(2*3)*4=1+24=25
    testValide("1*2*3+4", 10);   // (1*2*3)+4=6+4=10
    testValide("2+3*4+1", 15);   // 2+12+1=15
    testValide("3*4+2*5", 22);   // 12+10=22
    testValide("10+2*0",  10);   // 10+0=10
    testValide("0*5+3",   3);    // 0+3=3
}

// ─── Suite 5 : associativité gauche ─────────────────────────────────────────
// Vérifie la résolution du conflit en état 7, symbole + : reduce (r2)
// (associativité gauche de +) et en état 8 pour *.

void tests_associativite() {
    cout << "\n=== [5] Associativite gauche ===\n";
    // Addition
    testValide("1+2+3",            6);    // (1+2)+3
    testValide("10+20+30",         60);   // (10+20)+30
    testValide("1+2+3+4+5",        15);   // ((((1+2)+3)+4)+5)
    testValide("1+2+3+4+5+6+7+8+9+10", 55);
    // Multiplication
    testValide("2*3*4",            24);   // (2*3)*4
    testValide("2*3*4*5",          120);  // ((2*3)*4)*5
    testValide("1*2*3*4*5*6",      720);  // 6!
    // Mixte (associativité + priorité)
    testValide("1+2+3*4+5",        20);   // 1+2+12+5=20
    testValide("2*3+4*5+6*7",      68);   // 6+20+42=68
}

// ─── Suite 6 : parenthèses simples ──────────────────────────────────────────

void tests_parentheses_simples() {
    cout << "\n=== [6] Parentheses simples ===\n";
    testValide("(1+2)*3",          9);
    testValide("3*(2+4)",          18);
    testValide("(5+5)*(2+8)",      100);
    testValide("((3+4)*5)",        35);
    testValide("(1+2)*(3+4)",      21);
    testValide("(10+20)*(10+20)",  900);
    testValide("(0+1)*99",         99);
    testValide("99*(0+1)",         99);
    testValide("(1+0)*(1+0)",      1);
}

// ─── Suite 7 : parenthèses imbriquées profondes ──────────────────────────────
// Stress-test de la pile d'états : chaque ( empile Etat2.

void tests_parentheses_imbriquees() {
    cout << "\n=== [7] Parentheses imbriquees ===\n";
    testValide("(42)",             42);
    testValide("((42))",           42);
    testValide("(((42)))",         42);
    testValide("((((42))))",       42);
    testValide("(((((42)))))",     42);
    testValide("((((((42))))))",   42);
    testValide("(((((((42)))))))", 42);
    testValide("((1+2))",          3);
    testValide("(((1+2)))",        3);
    testValide("((1+2)*(3+4))",    21);
    testValide("(((1+2)*(3+4)))",  21);
}

// ─── Suite 8 : expressions complexes ────────────────────────────────────────
// Combinaisons profondes : priorité + associativité + parenthèses.

void tests_expressions_complexes() {
    cout << "\n=== [8] Expressions complexes ===\n";
    // (1+2*3)*(4+5*6) = 7*34 = 238
    testValide("(1+2*3)*(4+5*6)",              238);
    // 2*(3+4*(5+6)) = 2*(3+44) = 2*47 = 94
    testValide("2*(3+4*(5+6))",                94);
    // (1+2)*(3+4)*(5+6) = 3*7*11 = 231
    testValide("(1+2)*(3+4)*(5+6)",            231);
    // (2+3)*(4+5)*(6+7) = 5*9*13 = 585
    testValide("(2+3)*(4+5)*(6+7)",            585);
    // 1+2*3+4*5+6*7 = 1+6+20+42 = 69
    testValide("1+2*3+4*5+6*7",               69);
    // ((1+2)*3+(4*5))*6 = (9+20)*6 = 29*6 = 174
    testValide("((1+2)*3+4*5)*6",             174);
    // (10+2)*(10+2)*(10+2) = 12*12*12 = 1728
    testValide("(10+2)*(10+2)*(10+2)",        1728);
    // 3*(2*(1+4)) = 3*(2*5) = 3*10 = 30
    testValide("3*(2*(1+4))",                  30);
    // (1+1)*(2+2)*(3+3)*(4+4) = 2*4*6*8 = 384
    testValide("(1+1)*(2+2)*(3+3)*(4+4)",     384);
    // (100+200)*(100+200) = 300*300 = 90000
    testValide("(100+200)*(100+200)",          90000);
    // 100*100*100 = 1000000
    testValide("100*100*100",                  1000000);
}

// ─── Suite 9 : conflits LALR(1) ciblés ──────────────────────────────────────
// Chaque expression cible précisément un conflit de la table :
//   • Etat7, val+ : reduce r2  (assoc. gauche de +)
//   • Etat7, val* : shift  d5  (priorité * > +)
//   • Etat8, val+ : reduce r3  (assoc. gauche de *)
//   • Etat8, val* : reduce r3  (assoc. gauche de *)

void tests_conflits_lalr() {
    cout << "\n=== [9] Conflits LALR(1) ===\n";

    // -- Conflit Etat7/PLUS -> reduce r2 (E -> E+E, assoc. gauche)
    // Si la résolution était droite, 1+2+3 = 1+(2+3)=6 (même résultat ici)
    // Mais 1+2+3 doit être (1+2)+3 = 6.  Pour distinguer, pas de meilleur test
    // car + est commutatif : on se concentre sur les effets de bord de priorité.
    testValide("1+2+3",   6);   // assoc gauche vérifiée indirectement
    testValide("2+3+4",   9);

    // -- Conflit Etat7/MULT -> shift d5 (priorité * sur +)
    // CRUCIAL : si le conflit était mal résolu (reduce au lieu de shift),
    // "1+2*3" donnerait (1+2)*3=9 au lieu de 1+(2*3)=7.
    testValide("1+2*3",   7);   // doit être 7, PAS 9
    testValide("5+3*2",   11);  // doit être 11, PAS 16
    testValide("4+6*0",   4);   // 4+0=4, PAS 0

    // -- Conflit Etat8/PLUS -> reduce r3 (E -> E*E)
    // "2*3+4" : après "2*3" en état 8, le "+" doit déclencher reduce r3
    // donc on obtient (2*3)+4=10, PAS 2*(3+4)=14.
    testValide("2*3+4",   10);  // doit être 10, PAS 14
    testValide("3*3+1",   10);  // 9+1=10, PAS 3*4=12

    // -- Conflit Etat8/MULT -> reduce r3 (assoc. gauche de *)
    // "2*3*4" doit être (2*3)*4=24, PAS 2*(3*4)=24 (même ici mais le chemin diffère)
    testValide("2*3*4",   24);
    // Pour rendre le chemin observable, on imbrique avec +:
    // "(2*3)*4+1" vs "2*(3*4)+1" : les deux valent 25, mais
    // "2*3*4+1" doit valuer ((2*3)*4)+1 = 24+1 = 25
    testValide("2*3*4+1", 25);
}

// ─── Suite 10 : réutilisation de l'automate ─────────────────────────────────
// Vérifie que Analyser() réinitialise correctement les piles et l'état interne.

void tests_reutilisation_automate() {
    cout << "\n=== [10] Reutilisation du meme Automate ===\n";
    Automate a;

    // Alternance valide / invalide / valide
    testReutilisation(a,        "1+2",    3);
    testReutilisationInvalide(a, "+1");           // reset apres erreur
    testReutilisation(a,        "3*4",    12);
    testReutilisationInvalide(a, "1+");           // reset apres erreur
    testReutilisation(a,        "10+20",  30);
    testReutilisationInvalide(a, "()");
    testReutilisation(a,        "(1+2)*3", 9);

    // Plusieurs analyses valides successives
    testReutilisation(a, "5",     5);
    testReutilisation(a, "5+5",   10);
    testReutilisation(a, "5*5",   25);
    testReutilisation(a, "(5+5)*5", 50);

    // Résultat 0 après une expression nulle
    testReutilisation(a, "0*999", 0);
    testReutilisation(a, "1*1",   1);
}

// ─── Suite 11 : invalides simples ───────────────────────────────────────────

void tests_invalides_simples() {
    cout << "\n=== [11] Invalides simples ===\n";
    testInvalide("",       "vide");
    testInvalide("+1",     "operateur en debut");
    testInvalide("*1",     "operateur en debut");
    testInvalide("1+",     "operateur en fin");
    testInvalide("1*",     "operateur en fin");
    testInvalide("()",     "parentheses vides");
    testInvalide("(1+2",   "parenthese non fermee");
    testInvalide("1+2)",   "parenthese en trop");
    testInvalide("1++2",   "double operateur");
    testInvalide("1+*2",   "deux operateurs consecutifs");
    testInvalide("1**2",   "double operateur");
    testInvalide("abc",    "caracteres invalides");
    testInvalide("1 2",    "espace sans operateur");
    testInvalide("1+2 3",  "espace dans expression");
}

// ─── Suite 12 : invalides complexes ─────────────────────────────────────────
// Erreurs syntaxiques plus subtiles, dans des contextes imbriqués.

void tests_invalides_complexes() {
    cout << "\n=== [12] Invalides complexes ===\n";
    testInvalide("((1+2)",        "parenthese ouvrante en trop");
    testInvalide("(1+2))",        "parenthese fermante en trop");
    testInvalide("((1+2)+(3+4)",  "profond : parenthese non fermee");
    testInvalide("(1+2)*(3+4))",  "profond : parenthese fermante en trop");
    testInvalide("((()))",        "parentheses vides imbriquees");
    testInvalide("(+)",           "operateur seul entre parentheses");
    testInvalide("(*)",           "operateur seul entre parentheses");
    testInvalide("(1+)",          "operateur en fin dans parentheses");
    testInvalide("(1*)",          "operateur en fin dans parentheses");
    testInvalide("+",             "seul operateur");
    testInvalide("*",             "seul operateur");
    testInvalide("((",            "parentheses ouvrantes seulement");
    testInvalide("))",            "parentheses fermantes seulement");
    testInvalide("1+2*(3+",       "expression incomplete imbriquee");
    testInvalide(")*1",           "parenthese fermante en debut");
    testInvalide("1+(+2)",        "operateur en debut dans sous-expression");
    testInvalide("(1+2)*()",      "sous-expression vide");
    testInvalide("1+(2+3",        "sous-expression non fermee");
}

// ─── Main ────────────────────────────────────────────────────────────────────

int main() {
    cout << "==============================================\n";
    cout << "   Suite de tests - Analyseur LALR(1)        \n";
    cout << "==============================================\n";

    tests_valeurs_simples();
    tests_addition();
    tests_multiplication();
    tests_priorite();
    tests_associativite();
    tests_parentheses_simples();
    tests_parentheses_imbriquees();
    tests_expressions_complexes();
    tests_conflits_lalr();
    tests_reutilisation_automate();
    tests_invalides_simples();
    tests_invalides_complexes();

    int total = passed + failed;
    cout << "\n==============================================\n";
    cout << "  Resultats : " << passed << "/" << total << " passes\n";
    if (failed == 0)
        cout << "  Tous les tests sont PASSES !\n";
    else
        cout << "  " << failed << " test(s) ECHOUE(S).\n";
    cout << "==============================================\n";

    return (failed == 0) ? 0 : 1;
}
