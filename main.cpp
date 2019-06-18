
#include <iostream>
#include <string>

#include "App.h"


using std::cout, std::endl, std::string;


int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: ./spellchecker <dict> [files...]" << endl;
    }
    App app(argv[1], false);
    for (int i = 2; i < argc; ++i) {
        cout << endl << "Cheking file '" << argv[i] << "'. Unknown words:" << endl;
        app.run(argv[i]);
    }
    return 1;
}
