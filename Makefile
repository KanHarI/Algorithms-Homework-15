
CPPFLAGS=-O2 -std=c++17 -Wall


main: main.o
	g++ -o main main.o

main.o: main.cpp RBNode.h
	g++ $(CPPFLAGS) -c main.cpp

clean:
	rm main
	rm *.o
