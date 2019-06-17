
#include "hash.h"

using std::string;

constexpr size_t MULTIPLIER = 701;

size_t hash(std::string str) {
	// No great theory behind this, there is not much discussion about hashing
	// strings in the book (the algorithms which are discussed assume we can)
	// encode it as a giant number which will require the use of a bignum
	// arithmetic library :(
	size_t accumulator = 0;
	for (auto c : str) {
		// Shuffle bytes
#if __x86_64__ || __ppc64__
		accumulator = ((accumulator & 0xFFFFFFFF00000000) >> 32) | (accumulator << 32);
		accumulator = ((accumulator & 0xFF00FF00FF00FF00) >> 8) | ((accumulator & 0x00FF00FF00FF00FF) << 8);
#else
		accumulator = ((accumulator & 0xFFFF0000) >> 16) | (accumulator << 16);
#endif
		accumulator *= MULTIPLIER;
		accumulator += c;
	}
	accumulator *= MULTIPLIER;
	return accumulator;
}
