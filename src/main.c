#include "cbhe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage() {
	fprintf(stderr, "Usage: cbhe [flag] input_file output_file depth\n");
	fprintf(stderr, "\tflag - \n");
	fprintf(stderr, "\t\tuse -d or --decompress to decompress\n");
	fprintf(stderr, "\t\tuse -c or --compress to compress\n");
	fprintf(stderr, "\t\tif no flag is specified, the file will be compressed\n");
	fprintf(stderr, "\tinput_file - the path to the input file\n");
	fprintf(stderr, "\toutput_file - the path to the output file\n");
	fprintf(stderr, "\tdepth - the depth of the context\n");
	exit(1);
}

int main(int argc, char **argv) {

	CBHE_compress("src/main.c", "out.cbhe", 2);
	CBHE_decompress("out.cbhe", "test.c");
	return 0;

	if (argc == 5) {
		char *flag = argv[1];
		if (strcmp(flag, "-c") == 0 || strcmp(flag, "--compress") == 0) {
			int depth = atoi(argv[4]);
			CBHE_compress(argv[2], argv[3], depth);
		}
		else if (strcmp(flag, "-d") == 0 || strcmp(flag, "--decompress") == 0) {
			CBHE_decompress(argv[2], argv[3]);
		}
		else {
			usage();
		}
	}
	else if (argc == 4) { // by default compress if no flag is specified
		int depth = atoi(argv[3]);
		CBHE_compress(argv[1], argv[2], depth);
	}
	else {
		usage();
	}
	
	return 0;
}
