#include <iostream>
#include "automate.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <expression>" << endl;
        return 1;
    }

    string expression(argv[1]);
    Automate automate;
    
    if (automate.Analyser(expression)) {
        cout << automate.GetResultat() << endl;
        return 0;
    } else {
        cerr << "Erreur de syntaxe" << endl;
        return 1;
    }
}
