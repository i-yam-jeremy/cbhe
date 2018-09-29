#include "util.h"

/*
	Returns n to the m power
	@param n - the base
	@param m - the exponent (must be greater than or equal to zero)
	@return - n to the m power
*/
long CBHE_pow(int n, int m) {
	long result = 1;
	for (int i = 0; i < m; i++) {
		result *= n;
	}
	return result;
}
