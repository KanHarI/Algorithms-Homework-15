
#include <algorithm>

#include "FileReader.h"

using std::string;

FileReader::FileReader(std::string path)
	: m_file(path) {}

FileReader::~FileReader() {}

std::string FileReader::getWord() {
	while(1) { // Break by 'return' only when a word is found
		if(!m_line_reader) {
			string line;
			if(!getline(m_file,line)) {
				return "";
			}
			m_line_reader = std::istringstream(line);
		}
		string word;
		m_line_reader >> word;
		word = processWord(word);
		if (word != "") {
			return word;
		}
	}
}

string FileReader::processWord(string str) {
	// Erase non alphanumeric characters
	str.erase(std::remove_if(str.begin(), str.end(),
		std::not1(std::ptr_fun((int(*)(int))std::isalnum))), str.end());
	// covert to lower case
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}
