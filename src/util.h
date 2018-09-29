#ifndef __UTIL_H__
#define __UTIL_H__

/*
	The number of possible values that can fit inside a unsigned char data type
*/
#define CHAR_VALUE_COUNT 256

/*
	Returns n to the m power
	@param n - the base
	@param m - the exponent (must be greater than or equal to zero)
	@return - n to the m power
*/
long CBHE_pow(int n, int m);

#endif
