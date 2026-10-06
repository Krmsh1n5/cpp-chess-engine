CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2
OBJ      = jeu.o pieces.o main.o
BIN      = chess

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(OBJ)

jeu.o:    jeu.cpp jeu.h pieces.h
pieces.o: pieces.cpp pieces.h
main.o:   main.cpp jeu.h

clean:
	rm -f *.o $(BIN)

.PHONY: clean
