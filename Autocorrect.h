
#ifndef AUTOCORRECT_H
#define AUTOCORRECT_H

#include <memory>

#include "Hashtable.h"

class Autocorrect final {
public:
	Autocorrect(const Hashtable<std::string>& dict);
	~Autocorrect();

	// Try yo find a word that the author ment
	std::string attemptAutocorrect(std::string word);

private:
	// Find words with a single letter switched to a homophonoc letter
	std::string findHomophonicWords(std::string word);

	// Find words that are the same as given word, with one letter doubled
	std::string findLetterDoubledWords(std::string word);

	// Find words that are the same as given word, with a double letter
	// collapsed to single
	std::string findDoubledroppedWords(std::string word);

	// Find words that are the same as given word, with 2 consequtive
	// letters swaped
	std::string findSwapLetteredWords(std::string word);

	const Hashtable<std::string>& m_dict;
};

#endif
