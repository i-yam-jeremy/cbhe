#include "hufftree.h"

#include "util.h"

#include <stdlib.h>

/*
	Creates a Huffman tree from the given parameters
	@param c - the character this node represents (only for leaf nodes, -1 for non-leaf nodes)
	@param count - the count of this node
	@param left - the left child (NULL if this is leaf node)
	@param right - the right child (NULL if this is leaf node)
	@return - the created Huffman tree node
*/
CBHEHuffmanTree CBHE_new_huff_tree(int c, int count, CBHEHuffmanTree left, CBHEHuffmanTree right) {
	CBHEHuffmanTree tree = (CBHEHuffmanTree) malloc(sizeof(struct cbhe_huffman_tree));
	tree->c = c;
	tree->count = count;
	tree->left = left;
	tree->right = right;
	return tree;
}

/*
	Frees the Huffman tree node and all its children
	@param tree - the Huffman tree node
*/
void CBHE_free_huff_tree(CBHEHuffmanTree tree) {
	if (tree->left != NULL) {
		CBHE_free_huff_tree(tree->left);
	}
	if (tree->right != NULL) {
		CBHE_free_huff_tree(tree->right);
	}
	free(tree);
}

/*
	Frees all trees for all contexts
	@param trees - all trees
	@param depth - the depth of the context
*/
void CBHE_free_huff_trees(CBHEHuffmanTree *trees, int depth) {
	int tree_count = CBHE_pow(CHAR_VALUE_COUNT, depth-1);

	for (int i = 0; i < tree_count; i++) {
		if (trees[i] != NULL) {
			free(trees[i]);
		}
	}
}

/*
	Generates a single Huffman tree with the given index
	@param counts - the context-based counts
	@param depth - the depth of the context
	@param tree_index - the index of the tree
*/
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

/*
	Generates Huffman trees for each context based on the counts for each context
	@param counts - the counts for each context
	@param depth - the depth of the context
	@return - the Huffman trees
*/
CBHEHuffmanTree* CBHE_generate_trees(int *counts, int depth) {
	int tree_count = CBHE_pow(CHAR_VALUE_COUNT, depth-1);
	CBHEHuffmanTree *trees = (CBHEHuffmanTree*) malloc(sizeof(CBHEHuffmanTree)*tree_count);
	for (long i = 0; i < tree_count; i++) {
		trees[i] = CBHE_generate_tree(counts, depth, i);
	}
	return trees;
}
