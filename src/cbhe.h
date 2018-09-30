#ifndef __CBHE_H__
#define __CBHE_H__

/*
	Compresses the specified file using Context-Based Huffman Encoding (CBHE)
		and outputs the result in the output file
	@param input_file_path - the path to the input file
	@param output_file_path - the path to the output file
	@param depth - the depth of the context to use
*/
void CBHE_compress(char *input_file_path, char *output_file_path, int depth);

/*
	Decompresses the specified file using Context-Based Huffman Encoding (CBHE)
		and outputs the result in the output file
	@param input_file_path - the path to the input file
	@param output_file_path - the path to the output file
*/
void CBHE_decompress(char *input_file_path, char *output_file_path);

#endif
