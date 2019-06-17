
#include <iostream>

#include "App.h"
#include "FileReader.h"
#include "hash.h"

using std::cout, std::endl, std::string;

// 512K entries, probably around 10MB in RAM
// The dictionary has around 400K words
constexpr size_t HASH_TABLE_SIZE = 512*1024;

// negative result if str1<str2, 0 if same string, positive result if str1>str2
int strings_cmp_callback(string str1, string str2) {
    return str1.compare(str2);
}

App::App(std::string dict_path, bool suggestions)
    : m_suggestions(suggestions)
    , m_dict(hash, HASH_TABLE_SIZE) {
    // read_dict(dict_path);
}

App::~App() {}

void App::run(string checked_path) {
    m_words_tree = RBTree<string>::createTree(strings_cmp_callback);
    FileReader fr(checked_path);
    size_t line;
    string word;
    cout << "Reading input file..." << endl;
    tie(line, word)  = fr.getWord();
    while (word != "") {
        try {
            cout << "Inserting" << endl;
            m_words_tree->insert(word);
            cout << "Inserted" << endl;
        }
        // Words are double by design!
        catch (const KeyAlreadyExists& e) {}
        tie(line, word)  = fr.getWord();
    }
    
}

void App::read_dict(string dict_path) {
    FileReader fr(dict_path);
    size_t line;
    string word;
    cout << "Reading dictionaty..." << endl;
    tie(line, word)  = fr.getWord();
    while (word != "") {
        try {
            m_dict.insert(word);
        }
        // Some words are double in lower/upper case,
        // or they are the same as another word with non-alphanumeric characters in it.
        catch (const KeyAlreadyExists& e) {}
        tie(line, word)  = fr.getWord();
    }
    cout << "Finished loading dictionary" << endl;
}
