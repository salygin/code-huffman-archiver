## Description

- A fast, dependency‑free command‑line archiver that uses canonical Huffman codes for lossless data compression.

- Compress and decompress single or multi files

- Stream‑oriented encoder/decoder – works with arbitrarily large inputs

## Quick start

- ```archiver -c archive_name file1 [file2 ...]``` - compress ``` files file1, file2, ...``` and save the result to ```archive_name```.

- ```archiver -d archive_name``` - decompress files from ```archive_name``` and save to current directory.

- ```archiver -h``` - print help information.
