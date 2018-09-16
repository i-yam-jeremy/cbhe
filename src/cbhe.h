#ifndef __CBHE_H__
#define __CBHE_H__

namespace CBHE {

	class Params {
		public:
			Params(int depth);
		private:
			int depth; // how many characters of context to store
	};

	void compress(char *input_file, char *output_file, CBHE::Params params);
	void decompress(char *input_file, char *output_file);
}

#endif
