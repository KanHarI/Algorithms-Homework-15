
#include <iostream>
#include <string>

#include "App.h"


using std::cout, std::endl, std::string;


int main(int argc, char** argv) {
    App app("english-dict.txt", false);
    app.run("sample-sentance.txt");
    return 1;
}
