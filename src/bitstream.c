#include "bitstream.h"

#include <stdlib.h>

/*
	Creates a new bitstream around the given file
	@param file - the file
	@return - a bitstream
*/
CBHEBitstream CBHE_new_bitstream(FILE *file) {
	CBHEBitstream bs = (CBHEBitstream) malloc(sizeof(struct cbhe_bitstream));
	bs->file = file;
	bs->buffer = 0;
	bs->buffer_bit_count = 0;
	return bs;
}

/*
	Flushes the bitstream (writes the unwritten buffered data to the file and clears the buffer)
	@param bs - the bitstream
*/
void CBHE_flush_bitstream(CBHEBitstream bs) {
	unsigned char b = bs->buffer << (8 - bs->buffer_bit_count); // align so first bit is at top (most significant) end of byte
	fwrite(&b, sizeof(unsigned char), 1, bs->file);
	bs->buffer = 0;
	bs->buffer_bit_count = 0;
}

/*
	Writes a single bit to the output bitstream
	@param bs - the output bitstream
	@param bit - the bit to write (must be 0 or 1)
*/
void CBHE_write_bit(CBHEBitstream bs, int bit) {
	bs->buffer = (bs->buffer << 1) + bit;
	bs->buffer_bit_count++;

	if (bs->buffer_bit_count == 8) {
		fwrite(&bs->buffer, sizeof(unsigned char), 1, bs->file);
		bs->buffer = 0;
		bs->buffer_bit_count = 0;
	}
}

/*
	Writes a single bit to the input bitstream
	@param bs - the input bitstream
	@return - the bit that was read (0 or 1, OR -1 if EOF)
*/
int CBHE_read_bit(CBHEBitstream bs) {
	if (bs->buffer_bit_count == 0) {
		int bytes_read = fread(&bs->buffer, sizeof(unsigned char), 1, bs->file);
		if (bytes_read == 0) {
			return -1;
		}
		bs->buffer_bit_count = 8;
	}

	int bit = (bs->buffer >> (bs->buffer_bit_count-1)) & 0x1;
	bs->buffer_bit_count--;

	return bit;
}
