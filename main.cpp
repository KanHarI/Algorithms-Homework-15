
#include <iostream>
#include <string>

#include "RBTree.h"
#include "Hashtable.h"
#include "hash.h"

using std::cout, std::endl, std::string;

constexpr size_t HASH_TABLE_SIZE = 128*1024; // 128K entries, probably a few MB in RAM

/*
 * @return: negative integer if str1 comes before str2 in lexicographical order
 *          0 if equal
 *          positive integer if str2 comes before str1
 */
int strings_cmp_callback(string str1, string str2) {
    return str1.compare(str2);
}


int main(int argc, char** argv) {
    cout << "Creating tree" << endl;
    auto tree = RBTree<string>::createTree(strings_cmp_callback);
    tree->insert("BBA");
    tree->remove("BBA");
    tree->insert("BBA");
    tree->insert("BBB");
    tree->insert("BBC");
    tree->insert("BBD");
    tree->insert("BBE");
    tree->insert("BBF");
    tree->insert("BBG");
    tree->remove("BBD");
    cout << "All good - tree" << endl;
    Hashtable<string> hash_table(hash, HASH_TABLE_SIZE);
    hash_table.insert("KanHar");
}
