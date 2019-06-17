
#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>
#include <tuple>
#include <sstream>
#include <fstream>

class FileReader {
public:
	FileReader(std::string path);
	~FileReader();
	std::tuple<size_t, std::string> getWord();

private:
	static std::string processWord(std::string str);

	std::ifstream m_file;
	std::istringstream m_line_reader;
	size_t m_line;
};

#endif
