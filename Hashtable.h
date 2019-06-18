
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <list>
#include <vector>
#include <functional>

#include "Exceptions.h"

/**
 * This class is a a template implementation of a hashtable using user given 
 * hash function.
 * Collisions are solved via appending keys to a list in the relevant hash
 * table entry.
 */
template <class T>
class Hashtable final {
private:
	// Entry in the hash table
	class Entry;

public:
	// hash function turning objects of type T into hashes
	using hash_func_t = std::function<size_t(T)>;

	// Construct an empty hash table using hash_func as hashing function
	// and of size 'size'.
	Hashtable(hash_func_t hash_func, size_t size);
	~Hashtable();

	// Adds a key to the hash table
	void insert(T key);

	// Check whether a key is in the hash table
	bool lookup(T key) const;

private:
	// hash function
	hash_func_t m_hash_func;

	// Table linking hashed to entries
	std::vector<Entry> m_table;

	// size of hash table
	size_t m_size;
};

template <class T>
class Hashtable<T>::Entry final {
public:
	Entry();
	~Entry();

	// Insert a new key to the hash table
	void insert(T key);

	// Check whether a certain key is in the table
	bool lookup(T key) const;

private:
	// List of keys with current hash
	std::list<T> m_keys;
};

#include "Hashtable.hpp"

#endif
