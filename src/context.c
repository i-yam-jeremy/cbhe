#include "context.h"

/*
	Pushes the given value to the end of the buffer and shifts all other elements left one
	@param buffer - the context buffer of unsigned characters
	@param depth - the depth of the context
	@param c - the unsigned character to be pushed
*/
void CBHE_push_buffer(unsigned char *buffer, int depth, unsigned char c) {
	for (int i = 0; i < depth-1; i++) {
		buffer[i] = buffer[i+1];
	}
	buffer[depth-1] = c;
}

/*
	Returns the index in a context array (counts array or encodings array) from the given context buffer
	@return - the index in a context array (counts array or encodings array)
*/
long CBHE_get_context_index(unsigned char *buffer, int depth) {
	long index = 0;
	for (int i = 0; i < depth; i++) {
		index = (index << 8) + buffer[i];
	}
	return index;
}
