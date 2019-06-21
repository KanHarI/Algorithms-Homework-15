
#include "hash.h"

using std::string;

// Some prime number larger than a word and smaller then a dword
constexpr size_t MULTIPLIER = 1000003;

size_t hash(std::string str) {
	// No great theory behind this, there is not much discussion about hashing
	// strings in the book - the algorithms which are discussed assume we can
	// encode it as a giant number which will require the use of a bignum
	// arithmetic library :(
	// Hope my implementation is OK! (Debug prints show there are not too 
	// many collisions)
	size_t accumulator = 0;
	for (auto c : str) {
		// Reverse order of bytes to make sure information from 8 letters 
		// or more prior is lost (As bits only affect upper bits after 
		// multiplication). We do not want words that end in the same 8
		// letters to have the same hash!
#if __x86_64__ || __ppc64__ // 64 bit
		accumulator ^= ((accumulator & 0xFFFF000000000000) >> 48) ^ accumulator;
#else // 32 bit
		accumulator ^= ((accumulator & 0xFFFF0000) >> 16);
#endif
		accumulator *= MULTIPLIER;
		accumulator += c;
	}
	return accumulator;
}
