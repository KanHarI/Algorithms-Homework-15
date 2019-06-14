
#include <iostream>
#include <string>

#include "RBNode.h"

using std::cout, std::endl, std::string;

/*
 * @return: positive integer if str1 comes before str2 in lexicographical order
 *          0 if equal
 *          negative integer if str2 comes before str1
 */
int strings_cmp_callback(const string& str1,const string& str2) {
    return str2.compare(str1);
}

int main(int argc, char** argv) {
    auto tree = RBNode<string>::createRBTree(strings_cmp_callback);
    cout << "All good!" << endl;
    tree->insert("BBB");
    cout << "All good!" << endl;
    tree->insert("BBC");
    cout << "All good!" << endl;
    tree->insert("BBA");
    cout << "All good!" << endl;
}
