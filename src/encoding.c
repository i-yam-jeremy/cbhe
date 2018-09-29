#include "encoding.h"

#include "util.h"
#include "context.h"

#include <stdlib.h>

/*
	A recursive helper function for CBHE_flatten_tree
	@param encodings - the encodings buffer
	@param tree - the tree to flatten
	@param tree_index - the index of the tree
	@param bit_pattern - the current bit pattern at this point in the recursion of the tree
	@param bit_count - the bit count of the current bit pattern (the number of levels down you are in the tree)
*/
void CBHE_flatten_tree_helper(CBHEEncoding *encodings, CBHEHuffmanTree tree, int tree_index, long bit_pattern, int bit_count) {
	if (tree->left == NULL && tree->right == NULL)	{
		long encoding_index = (tree_index << 8) + tree->c;
		encodings[encoding_index].exists = 1;
		encodings[encoding_index].value = bit_pattern;
		encodings[encoding_index].bit_count = bit_count;
	}
	else {
		CBHE_flatten_tree_helper(encodings, tree->left, tree_index, (bit_pattern << 1) + 0, bit_count+1);
		CBHE_flatten_tree_helper(encodings, tree->right, tree_index, (bit_pattern << 1) + 1, bit_count+1);
	}
}

/*
	Flattens a single tree and puts the resultant encodings into the given encodings buffer
	@param encodings - the encodings buffer
	@param trees - all the Huffman trees
	@param tree_index - the index of the tree to flatten
*/
void CBHE_flatten_tree(CBHEEncoding *encodings, CBHEHuffmanTree *trees, int tree_index) {
	CBHE_flatten_tree_helper(encodings, trees[tree_index], tree_index, 0, 0);
}

/*
	Generate encodings based on the given Huffman trees
	@param trees - the Huffman trees for each context
	@param depth - the depth of the context
	@return - the encodings
*/
CBHEEncoding* CBHE_generate_encodings(CBHEHuffmanTree *trees, int depth) {
	CBHEEncoding *encodings = (CBHEEncoding*) calloc(CBHE_pow(CHAR_VALUE_COUNT, depth), sizeof(CBHEEncoding));

	for (long tree_index = 0; tree_index < CBHE_pow(CHAR_VALUE_COUNT, depth-1); tree_index++) {
		if (trees[tree_index] != NULL) {
			CBHE_flatten_tree(encodings, trees, tree_index);
		}
	}

	return encodings;
}

/*
	Writes a specific encoding bit pattern to the file
	@param output_bitstream - the output bitstream
	@param encoding - the encoding to write
*/
void CBHE_write_encoding(CBHEBitstream output_bitstream, CBHEEncoding encoding) {
	for (int i = encoding.bit_count-1; i >= 0; i--) {
		CBHE_write_bit(output_bitstream, (encoding.value >> i) & 0x1);
	}
}

/*
	Encodes the given input file using the given encodings and outputs the data
		to the output file
	@param encodings - the encodings context array
	@param depth - the depth of the context
	@param input - the input file
	@param output - the output file
*/
void CBHE_encode_file(CBHEEncoding *encodings, int depth, FILE *input, FILE *output) {
	unsigned char *buffer = (unsigned char*) calloc(depth, sizeof(unsigned char));
	CBHEBitstream output_bitstream = CBHE_new_bitstream(output);
	
	int c;
	while ((c = fgetc(input)) != -1) {
		CBHE_push_buffer(buffer, depth, c);
		long encoding_index = CBHE_get_context_index(buffer, depth);
		CBHE_write_encoding(output_bitstream, encodings[encoding_index]);
	}

	CBHE_flush_bitstream(output_bitstream);
	free(output_bitstream);
}
