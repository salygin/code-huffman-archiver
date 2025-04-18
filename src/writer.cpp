#include "writer.h"

Writer::Writer() : Writer(std::cout) {
}

Writer::Writer(std::ostream& out) : out(out), buffer(0), buffer_size(0) {
}

void Writer::write_1_bit(bool b) {
    //std::cerr << static_cast<int>(b);
    //std::cerr.flush();
    if (b) {
        buffer |= (1 << buffer_size);
    }
    buffer_size++;
    if (buffer_size == 8) {
        flush();
    }
}

void Writer::write_char(unsigned char c) {
    if (buffer_size == 0) {
        out << c;
        return;
    }
    for (size_t i = 0; i < 8; ++i) {
        write_1_bit(c >> i & 1);
    }
}

void Writer::write_9_bits(uint16_t b) {
    for (size_t i = 0; i < 9; ++i) {
        write_1_bit(b >> i & 1);
    }
    //std::cerr << std::endl;
}

void Writer::flush() {
    if (buffer_size > 0) {
        out << buffer;
        out.flush();
        buffer = 0;
        buffer_size = 0;
    }
}