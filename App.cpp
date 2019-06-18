
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

App::App(std::string dict_path)
    : m_dict(hash, HASH_TABLE_SIZE)
    , m_autocorrect(m_dict) {
    read_dict(dict_path);
}

App::~App() {}

void App::run(string checked_path) {
    std::shared_ptr<RBTree<std::string>> words_tree;
    words_tree = RBTree<string>::createTree(strings_cmp_callback);
    FileReader fr(checked_path);
    string word;
    cout << "Reading input file..." << endl;
    word = fr.getWord();
    while (word != "") {
        try {
            words_tree->insert(word);
        }
        // Words can appear more then once in text!
        catch (const KeyAlreadyExists& e) {}
        word  = fr.getWord();
    }
    cout << "Finished reading input file. Filtering words..." << endl;
    auto it = words_tree->minimum();
    while (it && !it->isNil()) {
        if (m_dict.lookup(it->get())) {
            it = it->kill();
        }
        else {
            it = it->succ();
        }
    }
    cout << "The following words are not in the dictionary:" << endl;
    it = words_tree->minimum();
    while (it && !it->isNil()) {
        word = it->get();
        cout << word << endl;
        auto suggestion = m_autocorrect.attemptAutocorrect(word);
        if (suggestion != "") {
            cout << "Did you mean: '" << suggestion << "'?" << endl;
        }
        it = it->succ();
    }
    
}

void App::read_dict(string dict_path) {
    FileReader fr(dict_path);
    cout << "Reading dictionaty..." << endl;
    string word = fr.getWord();
    while (word != "") {
        try {
            m_dict.insert(word);
        }
        // Some words are double in lower/upper case,
        // or they are the same as another word with non-alphanumeric characters in it.
        catch (const KeyAlreadyExists& e) {}
        word  = fr.getWord();
    }
    cout << "Finished loading dictionary" << endl;
}
