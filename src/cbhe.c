#include "cbhe.h"

#include <stdio.h>
#include <stdlib.h>

/*
	The number of possible values that can fit inside a char data type
*/
#define CHAR_VALUE_COUNT CBHE_pow(2, 8*sizeof(char))

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

/*
	Pushes the given value to the end of the buffer and shifts all other elements left one
	@param buffer - the context buffer of characters
	@param depth - the depth of the context
	@param c - the character to be pushed
*/
void CBHE_push_buffer(char *buffer, int depth, char c) {
	for (int i = 0; i < depth-1; i++) {
		buffer[i] = buffer[i+1];
	}
	buffer[depth-1] = c;
}

/*
	Returns the index in the context counts array from the given context buffer
	@return - the index in the counts array
*/
long CBHE_get_context_count_index(char *buffer, int depth) {
	long index = 0;
	for (int i = 0; i < depth; i++) {
		index += buffer[i] << (8*sizeof(char));
	}
	return index;
}

/*
	Increments the count specificied by the given context buffer
	@param counts - the counts
	@param buffer - the context buffer
	@param depth - the depth of the context
*/
void CBHE_increment_count(int *counts, char *buffer, int depth) {
	long index = CBHE_get_context_count_index(buffer, depth);
	counts[index]++;
}

/*
	Calculates the context-based counts based on the given input file
	@param input - the input file
	@param depth - the depth of the context
	@return - a N-dimensional array (of dimesion equal to the specified depth) indexed by
		characters in the context buffer
*/
int* CBHE_get_counts(FILE *input, int depth) {
	char *buffer = (char*) calloc(depth, sizeof(char));
	int *counts = (int*) calloc(CBHE_pow(CHAR_VALUE_COUNT, depth), sizeof(int));
	
	char c;
	while ((c = fgetc(input)) != -1) {
		CBHE_push_buffer(buffer, depth, c);
		CBHE_increment_count(counts, buffer, depth);
	}

	return counts;
}

//TODO make struct for huffman tree and make a huffman tree for each context

/*
	Compresses the specified file using Context-Based Huffman Encoding (CBHE)
		and outputs the result in the output file
	@param input_file_path - the path to the input file
	@param output_file_path - the path to the output file
	@param depth - the depth of the context to use
*/
void CBHE_compress(char *input_file_path, char *output_file_path, int depth) {
	FILE *input = fopen(input_file_path, "r");
	FILE *output = fopen(output_file_path, "w+");

	int *counts = CBHE_get_counts(input, depth);

	rewind(input);
	
	fclose(input);
	fclose(output);
}

/*
	Decompresses the specified file using Context-Based Huffman Encoding (CBHE)
		and outputs the result in the output file
	@param input_file_path - the path to the input file
	@param output_file_path - the path to the output file
*/
void CBHE_decompress(char *input_file, char *output_file) {
	// TODO
}
