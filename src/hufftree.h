#ifndef __HUFFTREE_H__
#define __HUFFTREE_H__

/*
	The root node of a Huffman tree
	c - the character this node represents (only for leaf nodes, -1 for non-leaf nodes)
	count - the count of this node
	left, right - the left and right child trees
*/
struct cbhe_huffman_tree {
	int c;
	int count;
	struct cbhe_huffman_tree *left, *right;
};
typedef struct cbhe_huffman_tree* CBHEHuffmanTree;

/*
	Frees all trees for all contexts
	@param trees - all trees
	@param depth - the depth of the context
*/
void CBHE_free_huff_trees(CBHEHuffmanTree *trees, int depth);

/*
	Generates Huffman trees for each context based on the counts for each context
	@param counts - the counts for each context
	@param depth - the depth of the context
	@return - the Huffman trees
*/
CBHEHuffmanTree* CBHE_generate_trees(int *counts, int depth);

#endif
