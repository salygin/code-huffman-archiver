#include "encoder.h"
#include "decoder.h"

#include <fstream>

void help_message() {
    std::cout << "Usage: archiver [options] file ..." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-c archive_name file1 [file2 ...] compresses files file1 ... into archive_name" << std::endl;
    std::cout << "-d archive_name                   decompresses archive_name" << std::endl;
    std::cout << "-h                                prints help message" << std::endl;
}

void incorrect() {
    std::cout << "Incorrect syntax." << std::endl;
    std::cout << "Type archiver -h to see help message" << std::endl;
}

int main(int argc, char** argv) {
    try {
        if (argc < 2) {
            incorrect();
            return 111;
        }
        std::string command = argv[1];
        if (command == "-c") {
            if (argc < 4) {
                incorrect();
                return 111;
            }
            std::string archive_name = static_cast<std::string>(argv[2]);
            std::vector<std::string> file_names(argv + 3, argv + argc);
            std::ofstream output_file(archive_name);
            if (!output_file) {
                throw std::runtime_error("Error: " + archive_name + "can't be opened");
            }
            Encoder encoder(file_names, output_file);
            output_file.close();
        } else if (command == "-d") {
            if (argc != 3) {
                incorrect();
                return 111;
            }
            std::string archive_name = static_cast<std::string>(argv[2]);
            std::ifstream input_file(archive_name);
            if (!input_file) {
                throw std::runtime_error("Error: " + archive_name + "can't be opened");
            }
            Decoder decoder(input_file);
            input_file.close();
        } else if (command == "-h") {
            if (argc != 2) {
                incorrect();
                return 111;
            }
            help_message();
        } else {
            incorrect();
            return 111;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 111;
    }
    return 0;
}
