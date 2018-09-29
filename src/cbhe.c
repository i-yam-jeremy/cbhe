#include "cbhe.h"

#include "util.h"
#include "hufftree.h"
#include "encoding.h"
#include "bitstream.h"
#include "context.h"

#include <stdio.h>
#include <stdlib.h>

/*
	Increments the count specificied by the given context buffer
	@param counts - the counts
	@param buffer - the context buffer
	@param depth - the depth of the context
*/
void CBHE_increment_count(int *counts, unsigned char *buffer, int depth) {
	long index = CBHE_get_context_index(buffer, depth);
	counts[index]++;
}

/*
	Calculates the context-based counts based on the given input file
	@param input - the input file
	@param depth - the depth of the context
	@return - a N-dimensional array (of dimesion equal to the specified depth) indexed by
		unsigned characters in the context buffer
*/
int* CBHE_get_counts(FILE *input, int depth) {
	unsigned char *buffer = (unsigned char*) calloc(depth, sizeof(unsigned char));
	int *counts = (int*) calloc(CBHE_pow(CHAR_VALUE_COUNT, depth), sizeof(int));

	int c;
	while ((c = fgetc(input)) != -1) {
		CBHE_push_buffer(buffer, depth, c);
		CBHE_increment_count(counts, buffer, depth);
	}

	return counts;
}

/*
	Writes the header data to the output file
	@param depth - the depth of the context
	@param output - the output file
*/
void CBHE_write_header(int depth, FILE *output) {
	fwrite("CBHE", sizeof(char), 4, output);
	fwrite(&depth, sizeof(int), 1, output);
}

/*
	Writes the counts of all characters in their given context to the output file.
	This data is necessary to rebuild the Huffman trees upon decoding.
	@param counts - the counts context array
	@param depth - the depth of the context
	@param output - the output file
*/
void CBHE_write_count_data(int *counts, int depth, FILE *output) {
	fwrite(counts, sizeof(int), CBHE_pow(CHAR_VALUE_COUNT, depth), output);
}

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

	CBHEHuffmanTree *trees = CBHE_generate_trees(counts, depth);


	CBHEEncoding *encodings = CBHE_generate_encodings(trees, depth);
	CBHE_free_huff_trees(trees, depth);


	CBHE_write_header(depth, output);
	CBHE_write_count_data(counts, depth, output);

	rewind(input);
	CBHE_encode_file(encodings, depth, input, output);
	
	free(encodings);

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
