#include "encoder.h"
#include "reader.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <queue>
#include <tuple>

HuffmanNode::HuffmanNode(size_t priority, Node* node) : priority(priority), node(node) {
}

bool HuffmanNode::operator<(const HuffmanNode& b) const {
    return std::tie(priority, node->c) > std::tie(b.priority, b.node->c);
}

Encoder::Encoder(std::vector<std::string> files) : Encoder(files, std::cout) {
}

Encoder::Encoder(std::vector<std::string> files, std::ostream& os) : files(files), out(Writer(os)) {
    for (size_t i = 0; i < files.size(); ++i) {
        if (i < files.size() - 1) {
            encode(files[i], ONE_MORE_FILE);
        } else {
            encode(files[i], ARCHIVE_END);
        }
    }
    out.flush();
}

void Encoder::write_code(std::vector<bool> code) {
    for (size_t i = 0; i < code.size(); ++i) {
        out.write_1_bit(code[i]);
    }
}

void dfs(Trie& trie, std::vector<uint16_t>& code_length, uint16_t h = 0) {
    if (trie.is_term()) {
        //std::cerr << (unsigned char)trie.get_char() << ' ' << h << std::endl;
        code_length[trie.get_char()] = h;
        return;
    }
    trie.move(0);
    dfs(trie, code_length, h + 1);
    trie.move_to_parent();
    trie.move(1);
    dfs(trie, code_length, h + 1);
    trie.move_to_parent();
}

struct SymbolLength {
    size_t c;
    uint16_t length;

    SymbolLength(size_t c, uint16_t length) : c(c), length(length) {
    }

    bool operator<(const SymbolLength& b) const {
        return std::tie(length, c) < std::tie(b.length, b.c);
    }
};

std::vector<std::vector<bool>> get_canonical_codes(std::vector<uint16_t> code_length) {
    std::vector<std::vector<bool>> canonical_codes(ALPH_SIZE);
    std::vector<SymbolLength> sorted_symbols;
    for (size_t i = 0; i < ALPH_SIZE; ++i) {
        if (code_length[i] > 0) {
            sorted_symbols.push_back(SymbolLength(i, code_length[i]));
        }
    }
    if (sorted_symbols.empty()) {
        return canonical_codes;
    }
    std::sort(sorted_symbols.begin(), sorted_symbols.end());
    canonical_codes[sorted_symbols[0].c] = std::vector<bool>(code_length[sorted_symbols[0].c], 0);
    for (size_t i = 1; i < sorted_symbols.size(); ++i) {
        size_t c = sorted_symbols[i].c;
        canonical_codes[c] = canonical_codes[sorted_symbols[i - 1].c];
        for (int32_t j = static_cast<int32_t>(canonical_codes[c].size()) - 1; j >= 0; --j) {
            if (canonical_codes[c][j] == 0) {
                canonical_codes[c][j] = 1;
                break;
            }
            canonical_codes[c][j] = 0;
        }
        if (code_length[c] > canonical_codes[c].size()) {
            canonical_codes[c].push_back(0);
        }
    }
    return canonical_codes;
}

struct SymbolCode {
    size_t c;
    std::vector<bool> code;

    SymbolCode(size_t c, std::vector<bool> code) : c(c), code(code) {
    }

    bool operator<(const SymbolCode& b) const {
        if (code.size() < b.code.size()) {
            return true;
        }
        if (code.size() > b.code.size()) {
            return false;
        }
        for (size_t i = 0; i < code.size(); ++i) {
            if (code[i] == 0 && b.code[i] == 1) {
                return true;
            }
            if (code[i] == 1 && b.code[i] == 0) {
                return false;
            }
        }
        return false;
    }
};

void Encoder::encode(std::string file_name, const uint16_t end) {
    std::ifstream input_file(file_name);
    Reader in(input_file);

    file_name = std::filesystem::path(file_name).filename().string();

    std::vector<size_t> number_of_symbols(ALPH_SIZE, 0);
    uint16_t symbol_count = 0;
    for (unsigned char c : file_name) {
        if (number_of_symbols[static_cast<size_t>(c)] == 0) {
            ++symbol_count;
        }
        ++number_of_symbols[static_cast<size_t>(c)];
    }
    while (in.file_not_ended()) {
        unsigned char c = in.read_char();
        if (number_of_symbols[static_cast<size_t>(c)] == 0) {
            ++symbol_count;
        }
        ++number_of_symbols[static_cast<size_t>(c)];
    }
    number_of_symbols[FILENAME_END] = 1;
    number_of_symbols[ONE_MORE_FILE] = 1;
    number_of_symbols[ARCHIVE_END] = 1;
    symbol_count += 3;
    //std::cerr << "symbol_count " << symbol_count << std::endl;
    out.write_9_bits(symbol_count);
    std::priority_queue<HuffmanNode> heap;
    for (size_t c = 0; c < ALPH_SIZE; ++c) {
        if (number_of_symbols[c]) {
            heap.push(HuffmanNode(number_of_symbols[c], new Node(static_cast<uint16_t>(c), true)));
        }
    }
    while (heap.size() > 1) {
        size_t parent_priority = heap.top().priority;
        Node* left = heap.top().node;
        heap.pop();
        parent_priority += heap.top().priority;
        Node* right = heap.top().node;
        heap.pop();
        //std::cerr << (unsigned char)left->c << ' ' << (unsigned char)right->c << std::endl;
        Node* parent = new Node(left, right, std::min(left->c, right->c), false);
        left->set_parent(parent);
        right->set_parent(parent);
        heap.push(HuffmanNode(parent_priority, parent));
    }
    Trie trie(heap.top().node);
    std::vector<uint16_t> code_length(ALPH_SIZE, 0);
    dfs(trie, code_length);
    size_t max_symbol_code_size = 0;
    for (size_t c = 0; c < ALPH_SIZE; ++c) {
        if (code_length[c] > max_symbol_code_size) {
            max_symbol_code_size = code_length[c];
        }
    }
    std::vector<std::vector<bool>> canonical_codes = get_canonical_codes(code_length);
    std::vector<SymbolCode> sorted_codes;
    for (size_t c = 0; c < ALPH_SIZE; ++c) {
        if (code_length[c] > 0) {
            /*
            std::cerr << (unsigned char)c << ' ';
            for (auto j : canonical_codes[c]) {
                std::cerr << j;
            }
            std::cerr << std::endl;
            */
            sorted_codes.push_back(SymbolCode(c, canonical_codes[c]));
        }
    }
    std::sort(sorted_codes.begin(), sorted_codes.end());
    for (auto i : sorted_codes) {
        out.write_9_bits(static_cast<uint16_t>(i.c));
    }
    std::vector<uint16_t> length_count(max_symbol_code_size + 1);
    for (size_t i = 0; i < ALPH_SIZE; ++i) {
        length_count[code_length[i]]++;
    }
    for (size_t i = 1; i <= max_symbol_code_size; ++i) {
        out.write_9_bits(length_count[i]);
    }
    for (auto c : file_name) {
        write_code(canonical_codes[static_cast<unsigned char>(c)]);
    }
    write_code(canonical_codes[FILENAME_END]);
    in.restart();
    while (in.file_not_ended()) {
        unsigned char c = in.read_char();
        /*
        std::cerr << c << ' ';
        for (auto j : canonical_codes[c]) {
            std::cerr << j;
        }
        std::cerr << std::endl;
        */
        write_code(canonical_codes[static_cast<size_t>(c)]);
    }
    write_code(canonical_codes[end]);
    input_file.close();
}
