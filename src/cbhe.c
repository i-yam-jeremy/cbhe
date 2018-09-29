#include "cbhe.h"

#include <stdio.h>
#include <stdlib.h>

/*
	The number of possible values that can fit inside a unsigned char data type
*/
#define CHAR_VALUE_COUNT 256


long CBHE_pow(int n, int m); // TODO FIXME

struct cbhe_huffman_tree {
	int c;
	int count;
	struct cbhe_huffman_tree *left, *right;
};

typedef struct cbhe_huffman_tree* CBHEHuffmanTree;

CBHEHuffmanTree CBHE_new_huff_tree(int c, int count, CBHEHuffmanTree left, CBHEHuffmanTree right) {
	CBHEHuffmanTree tree = (CBHEHuffmanTree) malloc(sizeof(struct cbhe_huffman_tree));
	tree->c = c;
	tree->count = count;
	tree->left = left;
	tree->right = right;
	return tree;
}

void CBHE_free_huff_tree(CBHEHuffmanTree tree) {
	if (tree->left != NULL) {
		CBHE_free_huff_tree(tree->left);
	}
	if (tree->right != NULL) {
		CBHE_free_huff_tree(tree->right);
	}
	free(tree);
}

void CBHE_free_huff_trees(CBHEHuffmanTree *trees, int depth) {
	int tree_count = CBHE_pow(CHAR_VALUE_COUNT, depth-1);

	for (int i = 0; i < tree_count; i++) {
		if (trees[i] != NULL) {
			free(trees[i]);
		}
	}
}

typedef struct {
	int exists;
	long value;
	int bit_count;
} CBHEEncoding;

CBHEEncoding CBHE_make_encoding(long value, int bit_count) {
	CBHEEncoding encoding;
	encoding.value = value;
	encoding.bit_count = bit_count;
	return encoding;
}

struct cbhe_bitstream {
	FILE *file;
	unsigned char buffer;
	int buffer_bit_count;
};

typedef struct cbhe_bitstream* CBHEBitstream;

CBHEBitstream CBHE_new_bitstream(FILE *file) {
	CBHEBitstream bs = (CBHEBitstream) malloc(sizeof(struct cbhe_bitstream));
	bs->file = file;
	bs->buffer = 0;
	bs->buffer_bit_count = 0;
	return bs;
}

void CBHE_flush_bitstream(CBHEBitstream bs) {
	unsigned char b = bs->buffer << (8 - bs->buffer_bit_count); // align so first bit is at top (most significant) end of byte
	fwrite(&b, sizeof(unsigned char), 1, bs->file);
	bs->buffer = 0;
	bs->buffer_bit_count = 0;
}

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
		index += buffer[i] << (8*i);
	}
	return index;
}

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


CBHEHuffmanTree CBHE_generate_tree(int *counts, int depth, long tree_index) {
	CBHEHuffmanTree trees[CHAR_VALUE_COUNT] = {NULL};
	
	int tree_count = 0;
	for (int c = 0; c < CHAR_VALUE_COUNT; c++) {
		long count_index = (tree_index << 8) + c;
		int count = counts[count_index];
		if (count > 0) {
			trees[c] = CBHE_new_huff_tree(c, count, NULL, NULL);
			tree_count++;
		}
	}

	if (tree_count == 0) {
		return NULL; // No unsigned characters occurred with the given context
	}

	while (tree_count > 1) {
		int smallest_node_index = -1;
		int second_smallest_node_index = -1;

		for (int i = 0; i < CHAR_VALUE_COUNT; i++) {
			if (trees[i] == NULL) {
				continue;
			}
			if (smallest_node_index == -1 || trees[i]->count < trees[smallest_node_index]->count) {
				second_smallest_node_index = smallest_node_index;
				smallest_node_index = i;
			}
			else if (second_smallest_node_index == -1 || trees[i]->count < trees[second_smallest_node_index]->count) {
				second_smallest_node_index = i;
			}
		}

		CBHEHuffmanTree new_tree = CBHE_new_huff_tree(
					0,
					trees[smallest_node_index]->count + trees[second_smallest_node_index]->count,
					trees[smallest_node_index],
					trees[second_smallest_node_index]);
		
		trees[smallest_node_index] = new_tree; // overwrite smallest node by setting new tree to smallest node
		trees[second_smallest_node_index] = NULL; // clear second smallest node
		tree_count--;
	}

	
	for (int i = 0; i < CHAR_VALUE_COUNT; i++) { // search trees for the last remaining one (the non-NULL one)
		if (trees[i] != NULL) { 
			return trees[i];
		}
	}

	return NULL; // should never be reached
}

CBHEHuffmanTree* CBHE_generate_trees(int *counts, int depth) {
	int tree_count = CBHE_pow(CHAR_VALUE_COUNT, depth-1);
	CBHEHuffmanTree *trees = (CBHEHuffmanTree*) malloc(sizeof(CBHEHuffmanTree)*tree_count);
	for (long i = 0; i < tree_count; i++) {
		trees[i] = CBHE_generate_tree(counts, depth, i);
	}
	return trees;
}

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

void CBHE_flatten_tree(CBHEEncoding *encodings, CBHEHuffmanTree *trees, int tree_index) {
	CBHE_flatten_tree_helper(encodings, trees[tree_index], tree_index, 0, 0);
}

CBHEEncoding* CBHE_generate_encodings(CBHEHuffmanTree *trees, int depth) {
	CBHEEncoding *encodings = (CBHEEncoding*) calloc(CBHE_pow(CHAR_VALUE_COUNT, depth), sizeof(CBHEEncoding));

	for (long tree_index = 0; tree_index < CBHE_pow(CHAR_VALUE_COUNT, depth-1); tree_index++) {
		if (trees[tree_index] != NULL) {
			CBHE_flatten_tree(encodings, trees, tree_index);
		}
	}

	return encodings;
}

void CBHE_write_bit(CBHEBitstream bs, int bit) {
	bs->buffer = (bs->buffer << 1) + bit;
	bs->buffer_bit_count++;

	if (bs->buffer_bit_count == 8) {
		fwrite(&bs->buffer, sizeof(unsigned char), 1, bs->file);
		bs->buffer = 0;
		bs->buffer_bit_count = 0;
	}
}

void CBHE_write_encoding(CBHEBitstream output_bitstream, CBHEEncoding encoding) {
	for (int i = encoding.bit_count-1; i >= 0; i--) {
		CBHE_write_bit(output_bitstream, (encoding.value >> i) & 0x1);
	}
}

void CBHE_encode(CBHEEncoding *encodings, int depth, FILE *input, FILE *output) {
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
	CBHE_encode(encodings, depth, input, output);
	
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
