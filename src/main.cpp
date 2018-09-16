#include "cbhe.h"

int main(int argc, char **argv) {
	
	compress("README.md", "out.cbhe", CBHE::Params(2));

	return 0;
}
