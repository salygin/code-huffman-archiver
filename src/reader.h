#include <cstdint>
#include <iostream>

class Reader {
public:
    Reader();

    Reader(std::istream& in);

    bool file_not_ended();

    bool read_1_bit();

    unsigned char read_char();

    uint16_t read_9_bits();

    void restart();

private:
    std::istream& in;
    unsigned char buffer;
    size_t buffer_size;
};