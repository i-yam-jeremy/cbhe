#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

#include <stdio.h>

/*
	A stream for writing and reading, one bit at a time
	@param file - the file this stream is reading from or writing to
	@param buffer - the byte buffer stored for intermediate bit writes (waits until 8 bits are written to write to file)
	@param buffer_bit_count - the number of bits in the buffer
*/
struct cbhe_bitstream {
	FILE *file;
	unsigned char buffer;
	int buffer_bit_count;
};

typedef struct cbhe_bitstream* CBHEBitstream;

/*
	Creates a new bitstream around the given file
	@param file - the file
	@return - a bitstream
*/
CBHEBitstream CBHE_new_bitstream(FILE *file);

/*
	Flushes the bitstream (writes the unwritten buffered data to the file and clears the buffer)
	@param bs - the bitstream
*/
void CBHE_flush_bitstream(CBHEBitstream bs);

/*
	Writes a single bit to the output bitstream
	@param bs - the output bitstream
	@param bit - the bit to write (must be 0 or 1)
*/
void CBHE_write_bit(CBHEBitstream bs, int bit);

#endif
