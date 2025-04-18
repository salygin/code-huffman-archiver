#include "reader.h"

#include <exception>

Reader::Reader() : Reader(std::cin) {
}

Reader::Reader(std::istream& in) : in(in), buffer(0), buffer_size(0) {
}

bool Reader::file_not_ended() {
    if (buffer_size == 0 && in.peek() == EOF) {
        return false;
    }
    return true;
}

bool Reader::read_1_bit() {
    if (buffer_size == 0) {
        buffer = in.get();
        if (buffer == static_cast<unsigned char>(EOF)) {
            throw std::runtime_error("reading from an empty file");
        }
        buffer_size = 8;
    }
    bool result = (buffer >> (8 - buffer_size) & 1);
    buffer_size--;
    //std::cerr << result;
    //std::cerr.flush();
    return result;
}

unsigned char Reader::read_char() {
    if (buffer_size == 0) {
        return in.get();
    }
    unsigned char result = 0;
    for (size_t i = 0; i < 8; ++i) {
        if (read_1_bit()) {
            result |= (1 << i);
        }
    }
    return result;
}

uint16_t Reader::read_9_bits() {
    uint16_t result = 0;
    for (size_t i = 0; i < 9; ++i) {
        if (read_1_bit()) {
            result |= (1 << i);
        }
    }
    //std::cerr << std::endl;
    return result;
}

void Reader::restart() {
    in.seekg(0, std::ios::beg);
}