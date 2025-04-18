#include <cstdint>
#include <iostream>

class Writer {
public:
    Writer();

    Writer(std::ostream& out);

    void write_1_bit(bool b);

    void write_char(unsigned char c);

    void write_9_bits(uint16_t b);

    void flush();

private:
    std::ostream& out;
    unsigned char buffer;
    size_t buffer_size;
};