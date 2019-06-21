
#include "hash.h"

using std::string;

// Some prime number larger than a word and smaller then a dword
constexpr size_t MULTIPLIER = 1000003;
constexpr size_t SALT = 0x99999999;

size_t hash(std::string str) {
	// No great theory behind this, there is not much discussion about hashing
	// strings in the book - the algorithms which are discussed assume we can
	// encode it as a giant number which will require the use of a bignum
	// arithmetic library :(
	// Hope my implementation is OK! (Debug prints show there are not too 
	// many collisions)
	size_t accumulator = SALT;
	for (auto c : str) {
		// Move entropy bits from unused high-end of hash to the low-end
#if __x86_64__ || __ppc64__ // 64 bit
		accumulator ^= ((accumulator & 0xFF00000000000000) >> 56) ^ ((accumulator & 0x00000000FF000000) >> 24);
#else // 32 bit
		accumulator ^= ((accumulator & 0xFF000000) >> 24);
#endif
		accumulator *= MULTIPLIER;
		accumulator += c;
	}
	return accumulator;
}
