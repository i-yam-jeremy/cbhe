# Context-Based Huffman Encoding (CBHE)
This is an attempt at a new data compression algorithm based on Huffman encoding. However, instead of a single Huffman tree being generated based on the frequencies of characters across the entire file, many trees are generated based on character frequencies relative to a fixed-sized buffer of previous characters.

## Pros
Allows for greater compression of the data because some characters are more likley to occur after others, allowing shorter codes for characters that occur commonly in local context but less often in the global scope (entire file). This even allows for having zero-length codes if one character is the only character that occurs after another.

## Cons
Since many Huffman trees are generated, the dictionary becomes very large. This makes it only useful for extremely large text files.

## Overall Review
This algorithm does not have much practical use, however I enjoyed created my own compression algorithm and have learned some so in the future I can make a better one.
