
#include <list>
#include <string>
#include <tuple>
#include <utility>

#include "Autocorrect.h"

using std::string, std::tuple, std::tie, std::list;

constexpr tuple<char,char> homophones[] = {
	{'a', 'e'},
	{'b', 'p'},
	{'c', 'k'},
	{'c', 's'},
	{'d', 't'},
	{'e', 'i'},
	{'e', 'y'},
	{'f', 'v'},
	{'g', 'j'},
	{'i', 'j'},
	{'i', 'y'},
	{'k', 'q'},
	{'l', 'r'},
	{'m', 'n'},
	{'o', 'u'},
	{'u', 'w'},
	{'x', 'z'}
};

static list<string> findHomophonicWordsBy(string word, char a, char b) {
	list<string> results;
	size_t idx = word.find(a);
	while (idx != string::npos) {
		results.push_back(word.substr(0, idx) + b + word.substr(idx+1, string::npos));
		idx = word.find(a, idx+1);
	}
	return results;
}

string Autocorrect::findHomophonicWords(string word) {
	for (auto t : homophones) {
		char a, b;
		tie(a, b) = t;
		for (auto mod_word : findHomophonicWordsBy(word, a, b)) {
			if (m_dict.lookup(mod_word)) {
				return mod_word;
			}
		}
		std::swap(a, b);
		for (auto mod_word : findHomophonicWordsBy(word, a, b)) {
			if (m_dict.lookup(mod_word)) {
				return mod_word;
			}
		}
	}
	return "";
}


string Autocorrect::findLetterDoubledWords(string word) {
	for (size_t i = 0; i < word.length()-1; ++i) {
		if (word[i] == word[i+1]) {
			string mod_word = word.substr(0, i) + word.substr(i+1, string::npos);
			if (m_dict.lookup(mod_word)) {
				return mod_word;
			}
		}
	}
	return "";
}

string Autocorrect::findDoubledroppedWords(string word) {
	for (size_t i = 0; i < word.length(); ++i) {
		string mod_word = word.substr(0, i) + word[i] + word.substr(i, string::npos);
		if (m_dict.lookup(mod_word)) {
			return mod_word;
		}
	}
	return "";
}

string Autocorrect::findSwapLetteredWords(string word) {
	for (size_t i = 0; i < word.length()-1; ++i) {
		string mod_word = word.substr(0, i) + word[i+1] + word[i] + word.substr(i+2, string::npos);
		if (m_dict.lookup(mod_word)) {
			return mod_word;
		}
	}
	return "";
}

Autocorrect::Autocorrect(const Hashtable<string>& dict)
	: m_dict(dict) {}

Autocorrect::~Autocorrect() {}

string Autocorrect::attemptAutocorrect(string word) {
	auto res = findLetterDoubledWords(word);
	if (res != "") {
		return res;
	}
	res = findSwapLetteredWords(word);
	if (res != "") {
		return res;
	}
	res = findDoubledroppedWords(word);
	if (res != "") {
		return res;
	}
	res = findHomophonicWords(word);
	if (res != "") {
		return res;
	}
	return "";
}
