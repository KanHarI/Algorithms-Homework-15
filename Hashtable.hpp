
#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

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
bool Hashtable<T>::Entry::lookup(T key) const {
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
bool Hashtable<T>::lookup(T key) const {
	size_t hash = m_hash_func(key);
	return m_table[hash % m_size].lookup(key);
}

#endif
