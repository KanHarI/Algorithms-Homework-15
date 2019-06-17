
CPPFLAGS=-O3 -march=native -std=c++17 -Wall -Wextra -DNDEBUG


main: main.o hash.o FileReader.o App.o
	g++ -o main main.o hash.o FileReader.o App.o

main.o: main.cpp *.h
	g++ $(CPPFLAGS) -c main.cpp

hash.o: hash.cpp *.h
	g++ $(CPPFLAGS) -c hash.cpp

FileReader.o: FileReader.cpp *.h
	g++ $(CPPFLAGS) -c FileReader.cpp

App.o: App.cpp *.h
	g++ $(CPPFLAGS) -c App.cpp

clean:
	rm main
	rm *.o
