
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <list>
#include <vector>
#include <functional>

#include "Exceptions.h"

template <class T>
class Hashtable final {
private:
	class Entry;
public:
	using hash_func_t = std::function<size_t(T)>;

	Hashtable(hash_func_t hash_func, size_t size);
	~Hashtable();

	// Adds a key to the hash table
	void insert(T key);
	// Check whether a key is in the hash table
	bool lookup(T key);

private:
	hash_func_t m_hash_func;
	std::vector<Entry> m_table;
	size_t m_size;
};

template <class T>
class Hashtable<T>::Entry final {
public:
	Entry();
	~Entry();

	void insert(T key);
	bool lookup(T key);

private:
	std::list<T> m_keys;
};

template <class T>
Hashtable<T>::Entry::Entry() {}

template <class T>
Hashtable<T>::Entry::~Entry() {}

template <class T>
void Hashtable<T>::Entry::insert(T key) {
	auto it = std::find(m_keys.begin(), m_keys.end(), key);
	if (it != m_keys.end()) {
		throw KeyAlreadyExists();
	}
	m_keys.push_front(key);
}

template <class T>
bool Hashtable<T>::Entry::lookup(T key) {
	auto it = std::find(m_keys.begin(), m_keys.end(), key);
	return it != m_keys.end();
}

template <class T>
Hashtable<T>::Hashtable(typename Hashtable<T>::hash_func_t hash_func, size_t size)
	: m_hash_func(hash_func)
	, m_table(size)
	, m_size(size) {}

template <class T>
Hashtable<T>::~Hashtable() {}


template <class T>
void Hashtable<T>::insert(T key) {
	size_t hash = m_hash_func(key);
	m_table[hash % m_size].insert(key);
}

template <class T>
bool Hashtable<T>::lookup(T key) {
	size_t hash = m_hash_func(key);
	return m_table[hash % m_size].lookup(key);
}

#endif
