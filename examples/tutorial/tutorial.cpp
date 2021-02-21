//! [ex_reader_include]
#include <iostream>
#include <coffi/coffi.hpp>

using namespace COFFI;

int main(int argc, char** argv)
{
    //! [ex_reader_include]
    if (argc != 2) {
        std::cout << "Usage: tutorial <coff_file>" << std::endl;
        return 1;
    }

    //! [ex_reader_create]
    // Create an coffi reader
    coffi reader;

    // Load COFF data
    if (!reader.load(argv[1])) {
        std::cout << "Can't find or process COFF file " << argv[1] << std::endl;
        return 2;
    }
    //! [ex_reader_create]

    //! [ex_reader_archi]
    // Print COFF file properties
    std::cout << "COFF file architecture: ";
    switch (reader.get_architecture()) {
    case COFFI_ARCHITECTURE_PE:
        if (reader.get_optional_header()) {
            if (reader.get_optional_header()->get_magic() ==
                OH_MAGIC_PE32PLUS) {
                std::cout << "Portable Executable PE32+" << std::endl;
            }
            else {
                std::cout << "Portable Executable PE32" << std::endl;
            }
        }
        else {
            std::cout << "Portable Executable" << std::endl;
        }
        break;
    case COFFI_ARCHITECTURE_CEVA:
        std::cout << "CEVA" << std::endl;
        break;
    case COFFI_ARCHITECTURE_TI:
        std::cout << "Texas Instruments" << std::endl;
        break;
    }
    //! [ex_reader_archi]

    //! [ex_reader_sections]
    // Print the COFF file sections info
    auto sec_num = reader.get_sections().size();
    std::cout << "Number of sections: " << sec_num << std::endl;
    for (auto sec : reader.get_sections()) {
        std::cout << "  [" << sec->get_index() << "] " << sec->get_name()
                  << "\t" << sec->get_data_size() << std::endl;
    }
    //! [ex_reader_sections]

    //! [ex_reader_symbols]
    // Print COFF file symbols info
    for (auto sym : *reader.get_symbols()) {
        std::cout << sym.get_index() << " " << sym.get_name() << " "
                  << sym.get_value() << std::endl;
    }
    //! [ex_reader_symbols]

    return 0;
}
