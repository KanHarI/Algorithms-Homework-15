
#include <iostream>
#include <string>

#include "RBTree.h"

using std::cout, std::endl, std::string;

/*
 * @return: negative integer if str1 comes before str2 in lexicographical order
 *          0 if equal
 *          positive integer if str2 comes before str1
 */
int strings_cmp_callback(const string& str1,const string& str2) {
    return str1.compare(str2);
}

int main(int argc, char** argv) {
    cout << "Creating tree" << endl;
    auto tree = RBTree<string>::createTree(strings_cmp_callback);
    tree->insert("BBA");
    tree->insert("BBB");
    tree->insert("BBC");
    tree->insert("BBD");
    tree->insert("BBE");
    tree->insert("BBF");
    tree->insert("BBG");
    tree->remove("BBD");
    cout << "All good" << endl;
}
