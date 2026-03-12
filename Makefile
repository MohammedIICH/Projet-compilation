CXX = g++
CXXFLAGS = -Wall -std=c++11
LDFLAGS =

TARGET  = analyseur
OBJECTS = main.o symbole.o lexer.o etat.o automate.o

# Objets partages (sans main.o) pour les tests
SHARED_OBJECTS = symbole.o lexer.o etat.o automate.o

# Cible par defaut
all: $(TARGET)

# Compilation de l'executable principal
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Compilation et execution des tests
test: tests
	./tests

tests: tests.o $(SHARED_OBJECTS)
	$(CXX) $(CXXFLAGS) -o tests tests.o $(SHARED_OBJECTS) $(LDFLAGS)

tests.o: tests.cpp automate.h
	$(CXX) $(CXXFLAGS) -c tests.cpp

# Compilation des fichiers objets
main.o: main.cpp automate.h
	$(CXX) $(CXXFLAGS) -c main.cpp

symbole.o: symbole.cpp symbole.h
	$(CXX) $(CXXFLAGS) -c symbole.cpp

lexer.o: lexer.cpp lexer.h symbole.h
	$(CXX) $(CXXFLAGS) -c lexer.cpp

etat.o: etat.cpp etat.h automate.h
	$(CXX) $(CXXFLAGS) -c etat.cpp

automate.o: automate.cpp automate.h etat.h
	$(CXX) $(CXXFLAGS) -c automate.cpp

# Nettoyage des fichiers generes
clean:
	rm -f $(OBJECTS) tests.o tests $(TARGET)

# Execution du programme
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run test
