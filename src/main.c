#include "cbhe.h"

int main(int argc, char **argv) {
	
	CBHE_compress("src/main.c", "out.cbhe", 2);

	return 0;
}
