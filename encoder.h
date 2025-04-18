#include "trie.h"
#include "writer.h"

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

const size_t ALPH_SIZE = 259;
const uint16_t FILENAME_END = 256;
const uint16_t ONE_MORE_FILE = 257;
const uint16_t ARCHIVE_END = 258;

struct HuffmanNode {
    size_t priority;
    Node* node;

    HuffmanNode(size_t priority, Node* node);

    bool operator<(const HuffmanNode& b) const;
};

class Encoder {
public:
    Encoder(std::vector<std::string> files);

    Encoder(std::vector<std::string> files, std::ostream& out);

    void write_code(std::vector<bool> code);

    void encode(std::string file_name, const uint16_t end);

private:
    std::vector<std::string> files;
    Writer out;
};

std::vector<std::vector<bool>> get_canonical_codes(std::vector<uint16_t> code_length);