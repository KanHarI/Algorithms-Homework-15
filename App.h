
#ifndef APP_H
#define APP_H

#include <string>

#include "Hashtable.h"
#include "RBTree.h"

class App {
public:
	App(std::string dict_path, bool suggestions);
	~App();

	void run(std::string checked_path);

private:
	void read_dict(std::string dict_path);

	bool m_suggestions;
	Hashtable<std::string> m_dict;
	std::shared_ptr<RBTree<std::string>> m_words_tree;
};

#endif
