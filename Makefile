
CPPFLAGS=-flto -fwhole-program -Ofast -march=native -std=c++17 -Wall -Wextra -Wshadow -Wstrict-aliasing -pedantic -DNDEBUG


spellChecker: link
	strip --strip-all spellChecker

link: main.o hash.o FileReader.o App.o Autocorrect.o
	g++ $(CPPFLAGS) -o spellChecker main.o hash.o FileReader.o App.o Autocorrect.o

main.o: main.cpp *.h *.hpp
	g++ $(CPPFLAGS) -c main.cpp

hash.o: hash.cpp *.h *.hpp
	g++ $(CPPFLAGS) -c hash.cpp

FileReader.o: FileReader.cpp *.h *.hpp
	g++ $(CPPFLAGS) -c FileReader.cpp

App.o: App.cpp *.h *.hpp
	g++ $(CPPFLAGS) -c App.cpp

Autocorrect.o: Autocorrect.cpp *.h *.hpp
	g++ $(CPPFLAGS) -c Autocorrect.cpp

clean:
	rm spellChecker
	rm *.o
