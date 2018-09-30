# Context-Based Huffman Encoding (CBHE)

## Building & Usage

### Building
 ```
 cd <cbhe directory>
 make
 ```
 
 ### Usage
 ```
 cbhe [flag] inputfile outputfile [depth]
 ```
 * `flag` is used for specifying whether to compress or decompress. To compress use `-c` or `--compress` and to decompress use `-d` or `--decompress`
 * `inputfile` is the input file. For compression mode, this is the uncompressed file. For decompression mode, this is the compressed file.
 * `outputfile` is the output file. For compression mode, this is the compressed file. For decompression mode, this is the uncompressed file.
 * `depth` is the size of the context in bytes. This is necessary for compression mode but is not necessary for decompression mode.
 
## CBHE Compressed File Specification
The extension used by CBHE compressed files is `.cbhe`. It is not required, but it is useful to be able to recognize that a given is a CBHE compressed file.  
  
The file is divided into three parts:
* Header
* Dictionary
* Body

### Header
The header is 16 bytes long. The header consists of: 
* 4 bytes representing the ASCII string "CBHE" (not null-terminated)
* 4 bytes representing the depth of the context as an integer
* 8 bytes representing the decompressed size of the original file

### Dictionary
The dictionary stores the count/frequency data of the characters that occurred within their given contexts. This is a sparse array so only non-zero values are stored.  
The dictionary starts with 8 bytes specifying how many non-zero counts will follow.  
Next, there is the count data. There is a count data entry for each non-zero count.  
Each count data entry is divided into two parts, the index and the count.
The index is depth bytes long and specifies the context to which this count refers.
The count is 4 bytes long and represents the number of times this context occurred.

### Body
The body is traditional Huffman encoding using prefix-free bit patterns. However, instead of using one set of Huffman codes, different sets of Huffman codes are used depending on the previous characters of context.

## The Algorithm
This is an attempt at a new data compression algorithm based on Huffman encoding. However, instead of a single Huffman tree being generated based on the frequencies of characters across the entire file, many trees are generated based on character frequencies relative to a fixed-sized buffer of previous characters.

### Pros
Allows for greater compression of the data because some characters are more likley to occur after others, allowing shorter codes for characters that occur commonly in local context but less often in the global scope (entire file). This even allows for having zero-length codes if one character is the only character that occurs after another.

### Cons
Since many Huffman trees are generated, the dictionary becomes very large. This makes it only useful for text files with lots of redundancy.

### Overall Review
This algorithm does not compete with commonly used compression algorithms like LZW, however I enjoyed created my own compression algorithm and have learned from it so in the future I can make a better one.
