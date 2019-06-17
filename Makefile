
CPPFLAGS=-O3 -march=native -std=c++17 -Wall -Wextra -DNDEBUG


main: main.o hash.o
	g++ -o main main.o hash.o

main.o: main.cpp RBTree.h Hashtable.h Exceptions.h
	g++ $(CPPFLAGS) -c main.cpp

hash.o: hash.cpp
	g++ $(CPPFLAGS) -c hash.cpp

clean:
	rm main
	rm *.o
