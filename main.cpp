
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
    cout << "Creating tree" << endl;
    auto tree = RBNode<string>::createRBTree(strings_cmp_callback);
    cout << "Tree created, inserting 'BBB'" << endl;
    tree->insert("BBB");
    cout << "inserting 'BBC'" << endl;
    tree->insert("BBC");
    cout << "inserting 'BBD'" << endl;
    tree->insert("BBD");
}
