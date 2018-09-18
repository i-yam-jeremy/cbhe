#include "cbhe.h"

#include <stdio.h>
#include <stdlib.h>

/*
	The number of possible values that can fit inside a char data type
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
	long value;
	int bit_count;
} CBHEEncoding;

CBHEEncoding CBHE_make_encoding(long value, int bit_count) {
	CBHEEncoding encoding;
	encoding.value = value;
	encoding.bit_count = bit_count;
	return encoding;
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
		return NULL; // No characters occurred with the given context
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

			CBHEHuffmanTree new_tree = CBHE_new_huff_tree(
						0,
						trees[smallest_node_index]->count + trees[second_smallest_node_index]->count,
						trees[smallest_node_index],
						trees[second_smallest_node_index]);
			
			trees[smallest_node_index] = new_tree; // overwrite smallest node by setting new tree to smallest node
			trees[second_smallest_node_index] = NULL; // clear second smallest node
		}
	}

	return trees[0];
}

CBHEHuffmanTree* CBHE_generate_trees(int *counts, int depth) {
	int tree_count = CBHE_pow(CHAR_VALUE_COUNT, depth-1);
	CBHEHuffmanTree *trees = (CBHEHuffmanTree*) malloc(sizeof(CBHEHuffmanTree)*tree_count);
	for (long i = 0; i < tree_count; i++) {
		trees[i] = CBHE_generate_tree(counts, depth, i);
	}
	return trees;
}

CBHEEncoding* CBHE_generate_encodings(CBHEHuffmanTree *trees, int depth) {
	return NULL; // TODO
}


void CBHE_encode(CBHEEncoding *encodings, FILE *input, FILE *output) {
	//TODO write CBHE_write_encoding(output, CBHEEncoding encoding)
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

	rewind(input);
	CBHE_encode(encodings, input, output);
	
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
