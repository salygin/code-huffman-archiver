#include "decoder.h"
#include "encoder.h"

#include <cassert>
#include <fstream>

Decoder::Decoder() : Decoder(std::cin) {
}

Decoder::Decoder(std::istream& in) : in(Reader(in)) {
    decode_1_file();
}

void Decoder::decode_1_file() {
    uint16_t symbols_count = in.read_9_bits();
    //std::cerr << symbols_count << std::endl;
    std::vector<uint16_t> sorted_symbols(symbols_count);
    for (size_t i = 0; i < symbols_count; ++i) {
        sorted_symbols[i] = in.read_9_bits();
        //std::cerr << static_cast<unsigned char>(sorted_symbols[i]);
    }
    //std::cerr << std::endl;
    std::vector<uint16_t> length_count(1, 0);
    uint16_t sum = 0;
    while (sum < symbols_count) {
        length_count.push_back(in.read_9_bits());
        sum += length_count.back();
    }
    std::vector<uint16_t> code_length(ALPH_SIZE, 0);
    size_t length = 1;
    for (auto i : sorted_symbols) {
        while (length_count[length] == 0) {
            ++length;
        }
        code_length[i] = length;
        --length_count[length];
    }
    std::vector<std::vector<bool>> canonical_codes = get_canonical_codes(code_length);
    Trie trie;
    for (size_t c = 0; c < ALPH_SIZE; ++c) {
        if (code_length[c] > 0) {
            /*
            std::cerr << static_cast<unsigned char>(c) << ' ';
            for (auto j : canonical_codes[c]) {
                std::cerr << j;
            }
            std::cerr << std::endl;
            */
            trie.add_code(canonical_codes[c], static_cast<uint16_t>(c));
        }
    }
    std::string file_name;
    while (true) {
        bool b = in.read_1_bit();
        trie.move(b);
        if (trie.is_term()) {
            uint16_t c = trie.get_char();
            trie.move_to_root();
            if (c == FILENAME_END) {
                break;
            }
            file_name += static_cast<char>(static_cast<unsigned char>(c));
        }
    }
    //std::cerr << file_name << std::endl;
    std::ofstream output_file(file_name);
    Writer out(output_file);
    while (true) {
        assert(in.file_not_ended());
        bool b = in.read_1_bit();
        trie.move(b);
        if (trie.is_term()) {
            uint16_t c = trie.get_char();
            trie.move_to_root();
            if (c == ONE_MORE_FILE) {
                decode_1_file();
                out.flush();
                output_file.close();
                break;
            }
            if (c == ARCHIVE_END) {
                out.flush();
                output_file.close();
                break;
            }
            out.write_char(static_cast<unsigned char>(c));
        }
    }
}