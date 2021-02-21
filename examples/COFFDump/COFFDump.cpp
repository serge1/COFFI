/*
Copyright (C) 2014-2014 by Serge Lamikhov-Center

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#define _SCL_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <map>
#include <list>
#include <ctime>

#include <coffi/coffi.hpp>

#define DUMP_DEC_FORMAT(width) \
    std::setw(width) << std::setfill(' ') << std::dec << std::right
#define DUMP_HEX_FORMAT(width) \
    std::setw(width) << std::setfill('0') << std::hex << std::right
#define DUMP_STR_FORMAT(width) \
    std::setw(width) << std::setfill(' ') << std::hex << std::left
using namespace COFFI;

#define I2X(value, signs) \
    DUMP_HEX_FORMAT(signs) << std::uppercase << static_cast<uint32_t>(value)

typedef struct section_flags_type
{
    uint32_t    flag;
    std::string descr;
} section_flags_type;

std::list<section_flags_type> section_flags_pe{
    {IMAGE_SCN_TYPE_NO_PAD, "NO_PAD"},
    {IMAGE_SCN_CNT_CODE, "CODE"},
    {IMAGE_SCN_CNT_INITIALIZED_DATA, "INITIALIZED_DATA"},
    {IMAGE_SCN_CNT_UNINITIALIZED_DATA, "UNINITIALIZED_DATA"},
    {IMAGE_SCN_MEM_DISCARDABLE, "DISCARDABLE"},
    {IMAGE_SCN_MEM_NOT_CACHED, "NOT_CACHED"},
    {IMAGE_SCN_MEM_NOT_PAGED, "NOT_PAGED"},
    {IMAGE_SCN_MEM_SHARED, "SHARED"},
    {IMAGE_SCN_MEM_EXECUTE, "EXECUTE"},
    {IMAGE_SCN_MEM_READ, "READ"},
    {IMAGE_SCN_MEM_WRITE, "WRITE"},
    {IMAGE_SCN_LNK_OTHER, "OTHER"},
    {IMAGE_SCN_LNK_INFO, "INFO"},
    {IMAGE_SCN_LNK_REMOVE, "REMOVE"},
    {IMAGE_SCN_LNK_COMDAT, "COMDAT"},
    {IMAGE_SCN_GPREL, "GPREL"},
    {IMAGE_SCN_MEM_PURGEABLE, "PURGEABLE"},
    {IMAGE_SCN_MEM_16BIT, "16BIT"},
    {IMAGE_SCN_MEM_LOCKED, "LOCKED"},
    {IMAGE_SCN_MEM_PRELOAD, "PRELOAD"},
    {IMAGE_SCN_ALIGN_1BYTES, "ALIGN_1BYTES"},
    {IMAGE_SCN_ALIGN_2BYTES, "ALIGN_2BYTES"},
    {IMAGE_SCN_ALIGN_4BYTES, "ALIGN_4BYTES"},
    {IMAGE_SCN_ALIGN_8BYTES, "ALIGN_8BYTES"},
    {IMAGE_SCN_ALIGN_16BYTES, "ALIGN_16BYTES"},
    {IMAGE_SCN_ALIGN_32BYTES, "ALIGN_32BYTES"},
    {IMAGE_SCN_ALIGN_64BYTES, "ALIGN_64BYTES"},
    {IMAGE_SCN_ALIGN_128BYTES, "ALIGN_128BYTES"},
    {IMAGE_SCN_ALIGN_256BYTES, "ALIGN_256BYTES"},
    {IMAGE_SCN_ALIGN_512BYTES, "ALIGN_512BYTES"},
    {IMAGE_SCN_ALIGN_1024BYTES, "ALIGN_1024BYTES"},
    {IMAGE_SCN_ALIGN_2048BYTES, "ALIGN_2048BYTES"},
    {IMAGE_SCN_ALIGN_4096BYTES, "ALIGN_4096BYTES"},
    {IMAGE_SCN_ALIGN_8192BYTES, "ALIGN_8192BYTES"},
    {IMAGE_SCN_LNK_NRELOC_OVFL, "NRELOC_OVFL"},
};

std::list<section_flags_type> section_flags_ti{
    // {STYP_REG, "REG"} This flag has value 0, it is not printed in the output
    {STYP_DSECT, "DSECT"},
    {STYP_NOLOAD, "NOLOAD"},
    {STYP_GROUP, "GROUP"},
    {STYP_PAD, "PAD"},
    {STYP_COPY, "COPY"},
    {STYP_TEXT, "TEXT"},
    {STYP_DATA, "DATA"},
    {STYP_BSS, "BSS"},
    {STYP_BLOCK, "BLOCK"},
    {STYP_PASS, "PASS"},
    {STYP_CLINK, "CLINK"},
    {STYP_VECTOR, "VECTOR"},
    {STYP_PADDED, "PADDED"},
    {STYP_ALIGN_2, "STYP_ALIGN_2"},
    {STYP_ALIGN_4, "STYP_ALIGN_4"},
    {STYP_ALIGN_8, "STYP_ALIGN_8"},
    {STYP_ALIGN_16, "STYP_ALIGN_16"},
    {STYP_ALIGN_32, "STYP_ALIGN_32"},
    {STYP_ALIGN_64, "STYP_ALIGN_64"},
    {STYP_ALIGN_128, "STYP_ALIGN_128"},
    {STYP_ALIGN_256, "STYP_ALIGN_256"},
    {STYP_ALIGN_512, "STYP_ALIGN_512"},
    {STYP_ALIGN_1024, "STYP_ALIGN_1024"},
    {STYP_ALIGN_2048, "STYP_ALIGN_2048"},
    {STYP_ALIGN_4096, "STYP_ALIGN_4096"},
    {STYP_ALIGN_8192, "STYP_ALIGN_8192"},
    {STYP_ALIGN_16384, "STYP_ALIGN_16384"},
    {STYP_ALIGN_32768, "STYP_ALIGN_32768"},
};

const std::map<coffi_architecture_t, std::list<section_flags_type>*>
    section_flags_per_architecture = {
        {COFFI_ARCHITECTURE_NONE, &section_flags_pe},
        {COFFI_ARCHITECTURE_PE, &section_flags_pe},
        {COFFI_ARCHITECTURE_CEVA, &section_flags_pe},
        {COFFI_ARCHITECTURE_TI, &section_flags_ti},
};

const std::map<coffi_architecture_t, uint32_t> alignment_mask_per_architecture{
    {COFFI_ARCHITECTURE_NONE, 0x00F00000},
    {COFFI_ARCHITECTURE_PE, 0x00F00000},
    {COFFI_ARCHITECTURE_CEVA, 0x00F00000},
    {COFFI_ARCHITECTURE_TI, 0x00000F00},
};

const std::string directory_names[] = {
    "EXPORT_TABLE",
    "IMPORT_TABLE",
    "RESOURCE_TABLE",
    "EXCEPTION_TABLE",
    "CERTIFICATE_TABLE",
    "BASE_RELOCATION_TABLE",
    "DEBUG",
    "ARCHITECTURE",
    "GLOBAL_PTR",
    "TLS_TABLE",
    "LOAD_CONFIG_TABLE",
    "BOUND_IMPORT",
    "IAT",
    "DELAY_IMPORT_DESCRIPTOR",
    "COMPLUS_RUNTIME_HEADER",
    "RESERVED",
};

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Syntax: " << argv[0] << " <file_name>" << std::endl;
        return 1;
    }

    coffi c;
    if (!c.load(argv[1])) {
        std::cout << "File '" << argv[1]
                  << "' is not a valid COFF file (or file was not found)"
                  << std::endl;
        return 2;
    }

    std::cout << "Dump of file " << argv[1] << std::endl;
    std::cout << std::endl;

    //------------------------------------------------------------------------------
    // Dump of COFF header
    //------------------------------------------------------------------------------
    if (c.get_header()) {
        std::cout << "File Header" << std::endl;
        if (c.get_architecture() == COFFI_ARCHITECTURE_TI) {
            std::cout << "  Target ID:                      "
                      << I2X(c.get_header()->get_target_id(), 4) << std::endl;
        }
        else {
            std::cout << "  Machine:                      "
                      << I2X(c.get_header()->get_machine(), 4) << std::endl;
        }
        std::cout << "  Number of Sections:           "
                  << I2X(c.get_header()->get_sections_count(), 4) << std::endl
                  << std::endl;
        std::time_t     t  = c.get_header()->get_time_data_stamp();
        struct std::tm* tt = std::localtime(&t);
        char            tstring[100];
        std::strftime(tstring, 100, "%Y-%m-%d %H:%M:%S", tt);
        std::cout << "  TimeDateStamp:                "
                  << I2X(c.get_header()->get_time_data_stamp(), 8) << " ("
                  << tstring << ")" << std::endl;
        std::cout << "  PointerToSymbolTable:         "
                  << I2X(c.get_header()->get_symbol_table_offset(), 8)
                  << std::endl;
        std::cout << "  NumberOfSymbols:              "
                  << I2X(c.get_header()->get_symbols_count(), 8) << std::endl;
        std::cout << "  SizeOfOptionalHeader:         "
                  << I2X(c.get_header()->get_optional_header_size(), 4)
                  << std::endl;
        std::cout << "  Characteristics:              "
                  << I2X(c.get_header()->get_flags(), 4) << std::endl;
        std::cout << std::endl;
    }

    //------------------------------------------------------------------------------
    // Dump of COFF optional header
    //------------------------------------------------------------------------------
    if (c.get_optional_header()) {
        std::cout << "Optional Header" << std::endl;
        std::cout << "  Magic                         "
                  << I2X(c.get_optional_header()->get_magic(), 4) << std::endl;
        if (c.get_architecture() == COFFI_ARCHITECTURE_TI) {
            std::cout << "  linker version                " << std::dec
                      << (int)c.get_optional_header()->get_linker_version()
                      << std::endl;
        }
        else {
            std::cout
                << "  linker version                " << std::dec
                << (int)c.get_optional_header()->get_major_linker_version()
                << "."
                << (int)c.get_optional_header()->get_minor_linker_version()
                << std::endl;
        }
        std::cout << "  size of code                  "
                  << I2X(c.get_optional_header()->get_code_size(), 4)
                  << std::endl;
        std::cout << "  size of initialized data      "
                  << I2X(c.get_optional_header()->get_initialized_data_size(),
                         8)
                  << std::endl;
        std::cout << "  size of uninitialized data    "
                  << I2X(c.get_optional_header()->get_uninitialized_data_size(),
                         8)
                  << std::endl;
        std::cout << "  entrypoint RVA                "
                  << I2X(c.get_optional_header()->get_entry_point_address(), 8)
                  << std::endl;
        std::cout << "  base of code                  "
                  << I2X(c.get_optional_header()->get_code_base(), 8)
                  << std::endl;
        if (c.get_optional_header()->get_magic() != OH_MAGIC_PE32PLUS) {
            std::cout << "  base of data                  "
                      << I2X(c.get_optional_header()->get_data_base(), 8)
                      << std::endl;
        }
        std::cout << std::endl;
    }

    //------------------------------------------------------------------------------
    // Dump of COFF Windows specific header
    //------------------------------------------------------------------------------
    if (c.get_win_header()) {
        std::cout << "  image base                    "
                  << I2X(c.get_win_header()->get_image_base(), 16) << std::endl;
        std::cout << "  section align                 "
                  << I2X(c.get_win_header()->get_section_alignment(), 8)
                  << std::endl;
        std::cout << "  file align                    "
                  << I2X(c.get_win_header()->get_file_alignment(), 8)
                  << std::endl;
        std::cout << "  required OS version           " << std::dec
                  << c.get_win_header()->get_major_os_version() << "."
                  << c.get_win_header()->get_minor_os_version() << std::endl;
        std::cout << "  image version                 " << std::dec
                  << c.get_win_header()->get_major_image_version() << "."
                  << c.get_win_header()->get_minor_image_version() << std::endl;
        std::cout << "  subsystem version             " << std::dec
                  << c.get_win_header()->get_major_subsystem_version() << "."
                  << c.get_win_header()->get_minor_subsystem_version()
                  << std::endl;
        std::cout << "  Win32 Version                 "
                  << c.get_win_header()->get_win32_version_value() << std::endl;
        std::cout << "  size of image                 "
                  << I2X(c.get_win_header()->get_image_size(), 8) << std::endl;
        std::cout << "  size of headers               "
                  << I2X(c.get_win_header()->get_headers_size(), 8)
                  << std::endl;
        std::cout << "  checksum                      "
                  << I2X(c.get_win_header()->get_checksum(), 8) << std::endl;
        std::cout << "  Subsystem                     "
                  << I2X(c.get_win_header()->get_subsystem(), 4) << std::endl;
        std::cout << "  DLL flags                     "
                  << I2X(c.get_win_header()->get_dll_flags(), 4) << std::endl;
        std::cout << "  stack reserve size            "
                  << I2X(c.get_win_header()->get_stack_reserve_size(), 16)
                  << std::endl;
        std::cout << "  stack commit size             "
                  << I2X(c.get_win_header()->get_stack_commit_size(), 16)
                  << std::endl;
        std::cout << "  heap reserve size             "
                  << I2X(c.get_win_header()->get_heap_reserve_size(), 16)
                  << std::endl;
        std::cout << "  heap commit size              "
                  << I2X(c.get_win_header()->get_heap_commit_size(), 16)
                  << std::endl;
        std::cout << "  RVAs & sizes                  "
                  << I2X(c.get_win_header()->get_number_of_rva_and_sizes(), 8)
                  << std::endl;
        std::cout << std::endl;
    }

    //------------------------------------------------------------------------------
    // Dump of Windows data directories
    //------------------------------------------------------------------------------
    const directories& dirs = c.get_directories();
    if (dirs.size() > 0) {
        std::cout << "Data Directory" << std::endl;
        for (auto dir : dirs) {
            std::cout << "  " << DUMP_STR_FORMAT(24)
                      << directory_names[dir->get_index()]
                      << "rva: " << I2X(dir->get_virtual_address(), 8)
                      << "  size: " << I2X(dir->get_size(), 8) << std::endl;
        }
        std::cout << std::endl;
    }

    //------------------------------------------------------------------------------
    // Dump of COFF sections
    //------------------------------------------------------------------------------
    uint32_t nb_relocs = 0;
    if (c.get_sections().size() > 0) {
        std::cout << "Section Table" << std::endl;
        for (auto sec : c.get_sections()) {

            uint32_t    virt_size;
            std::string virt_size_label = "VirtSize: ";
            std::string memory_page     = "";
            if (c.get_architecture() == COFFI_ARCHITECTURE_TI) {
                virt_size       = sec->get_physical_address();
                virt_size_label = "PhysAddr: ";
                memory_page =
                    "(page " + std::to_string(sec->get_page_number()) + ")";
            }
            else {
                virt_size = sec->get_virtual_size();
            }
            nb_relocs += sec->get_reloc_count();
            std::cout << "  " << I2X(sec->get_index(), 3) << " "
                      << DUMP_STR_FORMAT(9) << sec->get_name() << " "
                      << virt_size_label << I2X(virt_size, 8) << "  "
                      << "VirtAddr:  " << I2X(sec->get_virtual_address(), 8)
                      << memory_page << std::endl;
            std::cout << "    raw data offs:   "
                      << I2X(sec->get_data_offset(), 8)
                      << "  raw data size: " << I2X(sec->get_data_size(), 8)
                      << std::endl
                      << "    relocation offs: "
                      << I2X(sec->get_reloc_offset(), 8)
                      << "  relocations:   " << I2X(sec->get_reloc_count(), 8)
                      << std::endl;
            if (c.get_architecture() != COFFI_ARCHITECTURE_TI) {
                std::cout << "    line # offs:     "
                          << I2X(sec->get_line_num_offset(), 8)
                          << "  line #'s:      "
                          << I2X(sec->get_line_num_count(), 8) << std::endl;
            }
            std::cout << "    characteristics: " << I2X(sec->get_flags(), 8)
                      << std::endl;
            std::cout << "     ";

            auto section_flags_list =
                section_flags_per_architecture.at(c.get_architecture());
            uint32_t alignment_mask =
                alignment_mask_per_architecture.at(c.get_architecture());
            uint32_t flags = sec->get_flags();
            for (auto section_flags : *section_flags_list) {
                if ((section_flags.flag & alignment_mask)) {
                    if ((flags & alignment_mask) == section_flags.flag) {
                        std::cout << " " << section_flags.descr;
                    }
                }
                else {
                    if ((flags & section_flags.flag) == section_flags.flag) {
                        std::cout << " " << section_flags.descr;
                    }
                }
            }
            std::cout << std::endl << std::endl;
        }
        std::cout << std::endl;

        //------------------------------------------------------------------------------
        // Dump of symbols
        //------------------------------------------------------------------------------
        if (c.get_symbols()->size() > 0) {
            std::cout << "Symbol Table" << std::endl;
            std::cout << "  #    Value    Type Class Name" << std::endl;
            for (auto sym = c.get_symbols()->begin();
                 sym != c.get_symbols()->end(); sym++) {
                std::cout << "  " << I2X(sym->get_index(), 4) << " "
                          << I2X(sym->get_value(), 8) << " "
                          << I2X(sym->get_type(), 4) << " "
                          << I2X(sym->get_storage_class(), 2) << "    "
                          << sym->get_name() << std::endl;
                for (auto a = sym->get_auxiliary_symbols().begin();
                     a != sym->get_auxiliary_symbols().end(); a++) {
                    std::cout << "  AUX ";
                    for (size_t i = 0; i < sizeof(symbol_record); i++) {
                        std::cout << " "
                                  << I2X(static_cast<uint8_t>(a->value[i]), 2);
                    }
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
        }

        //------------------------------------------------------------------------------
        // Dump of relocation entries
        //------------------------------------------------------------------------------
        if (nb_relocs > 0) {
            std::cout << "Relocation Entries" << std::endl;
            std::cout << "  Section      Address  Type     Symbol" << std::endl;
            for (auto sec : c.get_sections()) {
                const std::vector<relocation>& relocs = sec->get_relocations();
                for (auto r : relocs) {
                    std::cout << "  " << I2X(sec->get_index(), 3) << " "
                              << DUMP_STR_FORMAT(8) << sec->get_name() << " "
                              << I2X(r.get_virtual_address(), 8) << " "
                              << I2X(r.get_type(), 8) << " "
                              << DUMP_STR_FORMAT(1) << r.get_symbol()
                              << std::endl;
                }
            }
            std::cout << std::endl;
        }
    }

    return 0;
}
