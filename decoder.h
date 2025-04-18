#include "reader.h"

#include <cstddef>
#include <string>
#include <vector>

class Decoder {
public:
    Decoder();

    Decoder(std::istream& in);

    void decode_1_file();

private:
    Reader in;
};