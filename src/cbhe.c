#include "cbhe.h"

#include "util.h"
#include "hufftree.h"
#include "encoding.h"
#include "bitstream.h"
#include "context.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	@param decompressed_size - the location to store the decompressed size
	@return - a N-dimensional array (of dimesion equal to the specified depth) indexed by
		unsigned characters in the context buffer
*/
int* CBHE_get_counts(FILE *input, int depth, long *decompressed_size) {
	unsigned char *buffer = (unsigned char*) calloc(depth, sizeof(unsigned char));
	int *counts = (int*) calloc(CBHE_pow(CHAR_VALUE_COUNT, depth), sizeof(int));

	*decompressed_size = 0;
	int c;
	while ((c = fgetc(input)) != -1) {
		CBHE_push_buffer(buffer, depth, c);
		CBHE_increment_count(counts, buffer, depth);
		(*decompressed_size)++;
	}

	return counts;
}

/*
	Writes the header data to the output file
	@param depth - the depth of the context
	@param decompressed_size - the decompressed size of the file
	@param output - the output file
*/
void CBHE_write_header(int depth, int decompressed_size, FILE *output) {
	fwrite("CBHE", sizeof(char), 4, output);
	fwrite(&depth, sizeof(int), 1, output);
	fwrite(&decompressed_size, sizeof(long), 1, output);
}

/*
	Checks if the header is valid, if it is not it
		prints an error message and exits
	@param input - the input file
*/
void CBHE_verify_header(FILE *input) {
	char *buffer = (char*) malloc(sizeof(char)*5);
	buffer[4] = '\0';
	fread(buffer, sizeof(char), 4, input);
	
	if (strcmp(buffer, "CBHE") != 0) {
		fprintf(stderr, "Invalid header. File did not start with \"CBHE\"");
		exit(1);
	}
}

/*
	Reads the data from the header of the given file
	@param input - the input file
	@param depth - the location to store the depth
	@param decompressed size - the location to store the decompressed size
*/
void CBHE_read_header(FILE* input, int *depth, long *decompressed_size) {
	CBHE_verify_header(input);
	fread(depth, sizeof(int), 1, input);
	fread(decompressed_size, sizeof(long), 1, input);
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
	Reads the counts of all characters in their given context from the input file
	@param input - the input file
	@param depth - the depth of the context
	@return - the counts context array
*/
int* CBHE_read_count_data(FILE *input, int depth) {
	long counts_buffer_length = CBHE_pow(CHAR_VALUE_COUNT, depth);
	int *counts = (int*) malloc(sizeof(int)*counts_buffer_length);
	fread(counts, sizeof(int), counts_buffer_length, input);
	return counts;
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

	long decompressed_size;
	int *counts = CBHE_get_counts(input, depth, &decompressed_size);

	CBHEHuffmanTree *trees = CBHE_generate_trees(counts, depth);


	CBHEEncoding *encodings = CBHE_generate_encodings(trees, depth);
	CBHE_free_huff_trees(trees, depth);


	CBHE_write_header(depth, decompressed_size, output);
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
void CBHE_decompress(char *input_file_path, char *output_file_path) {
	FILE *input = fopen(input_file_path, "r");
	FILE *output = fopen(output_file_path, "w+");

	int depth;
	long decompressed_size;
	CBHE_read_header(input, &depth, &decompressed_size);

	int *counts = CBHE_read_count_data(input, depth);

	CBHEHuffmanTree *trees = CBHE_generate_trees(counts, depth);

	CBHE_decode_file(trees, depth, decompressed_size, input, output);

	CBHE_free_huff_trees(trees, depth);

	fclose(input);
	fclose(output);
}
