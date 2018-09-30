#include "cbhe.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage() {
	fprintf(stderr, "Usage: cbhe [flag] input_file output_file depth\n");
	fprintf(stderr, "\tflag - \n");
	fprintf(stderr, "\t\tuse -d or --decompress to decompress\n");
	fprintf(stderr, "\t\tuse -c or --compress to compress\n");
	fprintf(stderr, "\tinput_file - the path to the input file\n");
	fprintf(stderr, "\toutput_file - the path to the output file\n");
	fprintf(stderr, "\tdepth - the depth of the context\n");
	exit(1);
}

int main(int argc, char **argv) {

	if (argc == 4 || argc == 5) {
		char *flag = argv[1];
		if (strcmp(flag, "-c") == 0 || strcmp(flag, "--compress") == 0) {
			if (argc != 5) {
				usage();
			}
			int depth = atoi(argv[4]);
			CBHE_compress(argv[2], argv[3], depth);
		}
		else if (strcmp(flag, "-d") == 0 || strcmp(flag, "--decompress") == 0) {
			if (argc != 4) {
				usage();
			}
			CBHE_decompress(argv[2], argv[3]);
		}
		else {
			usage();
		}
	}
	else {
		usage();
	}
	
	return 0;
}
