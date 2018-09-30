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

## The Algorithm
This is an attempt at a new data compression algorithm based on Huffman encoding. However, instead of a single Huffman tree being generated based on the frequencies of characters across the entire file, many trees are generated based on character frequencies relative to a fixed-sized buffer of previous characters.

### Pros
Allows for greater compression of the data because some characters are more likley to occur after others, allowing shorter codes for characters that occur commonly in local context but less often in the global scope (entire file). This even allows for having zero-length codes if one character is the only character that occurs after another.

### Cons
Since many Huffman trees are generated, the dictionary becomes very large. This makes it only useful for extremely large text files.

### Overall Review
This algorithm does not have much practical use, however I enjoyed created my own compression algorithm and have learned from it so in the future I can make a better one.
