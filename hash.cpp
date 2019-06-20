
#include "hash.h"

using std::string;

// Some prime number
constexpr size_t MULTIPLIER = 701;

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
		accumulator = ((accumulator & 0xFFFFFFFF00000000) >> 32) | (accumulator << 32);
		accumulator = ((accumulator & 0xFFFF0000FFFF0000) >> 16) | ((accumulator & 0x0000FFFF0000FFFF) << 16);
		accumulator = ((accumulator & 0xFF00FF00FF00FF00) >> 8) | ((accumulator & 0x00FF00FF00FF00FF) << 8);
		accumulator = ((accumulator & 0xF0F0F0F0F0F0F0F0) >> 4) | ((accumulator & 0x0F0F0F0F0F0F0F0F) << 4);
		accumulator = ((accumulator & 0xCCCCCCCCCCCCCCCC) >> 2) | ((accumulator & 0x3333333333333333) << 2);
		accumulator = ((accumulator & 0xAAAAAAAAAAAAAAAA) >> 1) | ((accumulator & 0x5555555555555555) << 1);
#else
		accumulator = ((accumulator & 0xFFFF0000) >> 16) | (accumulator << 16);
		accumulator = ((accumulator & 0xFF00FF00) >> 8) | ((accumulator & 0x00FF00FF) << 8);
		accumulator = ((accumulator & 0xF0F0F0F0) >> 4) | ((accumulator & 0x0F0F0F0F) << 4);
		accumulator = ((accumulator & 0xCCCCCCCC) >> 2) | ((accumulator & 0x33333333) << 2);
		accumulator = ((accumulator & 0xAAAAAAAA) >> 1) | ((accumulator & 0x55555555) << 1);
#endif
		accumulator *= MULTIPLIER;
		accumulator += c;
	}
	accumulator *= MULTIPLIER;
	return accumulator;
}
