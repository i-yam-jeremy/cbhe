#ifndef __ENCODING_H__
#define __ENCODING_H__

#include "hufftree.h"
#include "bitstream.h"

/*
	A flattened Huffman encoding
	@param exists - 1 iff the encoding exists (is initialized and valid), otherwise 0
	@param value - the bit pattern
	@param bit_count - the length (in bits) of the bit pattern
*/
typedef struct {
	int exists;
	long value;
	int bit_count;
} CBHEEncoding;

/*
	Generate encodings based on the given Huffman trees
	@param trees - the Huffman trees for each context
	@param depth - the depth of the context
	@return - the encodings
*/
CBHEEncoding* CBHE_generate_encodings(CBHEHuffmanTree *trees, int depth);

/*
	Encodes the given input file using the given encodings and outputs the data
		to the output file
	@param encodings - the encodings context array
	@param depth - the depth of the context
	@param input - the input file
	@param output - the output file
*/
void CBHE_encode_file(CBHEEncoding *encodings, int depth, FILE *input, FILE *output);

/*
	Decodes the given input file using the given encodings and outputs the data
		to the output file
	@param trees - the Huffman tree for each context
	@param depth - the depth of the context
	@param decompressed_size - the size of the decompressed file
	@param input - the input file
	@param output - the output file
*/
void CBHE_decode_file(CBHEHuffmanTree *trees, int depth, long decompressed_size, FILE *input, FILE *output);

#endif
