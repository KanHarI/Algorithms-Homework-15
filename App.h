
#ifndef APP_H
#define APP_H

#include <string>

#include "Autocorrect.h"
#include "Hashtable.h"
#include "RBTree.h"

class App final {
public:
	App(std::string dict_path);
	~App();

	void run(std::string checked_path);

private:
	void read_dict(std::string dict_path);

	bool m_suggestions;
	Hashtable<std::string> m_dict;
	Autocorrect m_autocorrect;
};

#endif
