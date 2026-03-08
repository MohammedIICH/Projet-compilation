CXX = g++
CXXFLAGS = -Wall -std=c++11
LDFLAGS =

TARGET = analyseur
OBJECTS = main.o symbole.o lexer.o automate.o

# Cible par defaut
all: $(TARGET)

# Compilation de l'executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Compilation des fichiers objets
main.o: main.cpp automate.h
	$(CXX) $(CXXFLAGS) -c main.cpp

symbole.o: symbole.cpp symbole.h
	$(CXX) $(CXXFLAGS) -c symbole.cpp

lexer.o: lexer.cpp lexer.h symbole.h
	$(CXX) $(CXXFLAGS) -c lexer.cpp

automate.o: automate.cpp automate.h
	$(CXX) $(CXXFLAGS) -c automate.cpp

# Nettoyage des fichiers generes
clean:
	rm -f $(OBJECTS) $(TARGET)

# Execution du programme
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
