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

#ifndef COFFI_TYPES_HPP
#define COFFI_TYPES_HPP

#include <string>
#include <vector>

namespace COFFI {

#ifndef COFFI_NO_OWN_TYPES
#if !defined(COFFI_NO_CSTDINT) && !defined(COFFI_NO_INTTYPES)
#include <stdint.h>
#else
    typedef unsigned char  uint8_t;
    typedef signed char    int8_t;
    typedef uint16_t uint16_t;
    typedef signed short   int16_t;
#ifdef _MSC_VER
    typedef unsigned __int32 uint32_t;
    typedef signed   __int32 int32_t;
    typedef unsigned __int64 uint64_t;
    typedef signed   __int64 int64_t;
#else
    typedef unsigned int       uint32_t;
    typedef signed   int       int32_t;
    typedef unsigned long long uint64_t;
    typedef signed   long long int64_t;
#endif // _MSC_VER
#endif // COFFI_NO_CSTDINT
#endif // COFFI_NO_OWN_TYPES

    // Identification index
#define CI_MAG0    0
#define CI_MAG1    1
#define CI_MAG2    0
#define CI_MAG3    1
#define CI_MAG4    2
#define CI_MAG5    3
#define CI_NIDENT0 2
#define CI_NIDENT1 4

    // Magic numbers
#define PEMAG0 'M'
#define PEMAG1 'Z'
#define PEMAG2 'P'
#define PEMAG3 'E'
#define PEMAG4   0
#define PEMAG5   0

    // Magic numbers of optional header
#define OH_MAGIC_PE32     0x10B
#define OH_MAGIC_PE32ROM  0x107
#define OH_MAGIC_PE32PLUS 0x20B

    // Machine Types
#define IMAGE_FILE_MACHINE_UNKNOWN   0x0000  // The contents of this field are assumed to be applicable to any machine type
#define IMAGE_FILE_MACHINE_AM33      0x01d3  // Matsushita AM33
#define IMAGE_FILE_MACHINE_AMD64     0x8664  // x64
#define IMAGE_FILE_MACHINE_ARM       0x01c0  // ARM little endian
#define IMAGE_FILE_MACHINE_ARMNT     0x01c4  // ARMv7( or higher ) Thumb mode only
#define IMAGE_FILE_MACHINE_ARM64     0xaa64  // ARMv8 in 64 - bit mode
#define IMAGE_FILE_MACHINE_EBC       0x0ebc  // EFI byte code
#define IMAGE_FILE_MACHINE_I386      0x014c  // Intel 386 or later processors and compatible processors
#define IMAGE_FILE_MACHINE_IA64      0x0200  // Intel Itanium processor family
#define IMAGE_FILE_MACHINE_M32R      0x9041  // Mitsubishi M32R little endian
#define IMAGE_FILE_MACHINE_MIPS16    0x0266  // MIPS16
#define IMAGE_FILE_MACHINE_MIPSFPU   0x0366  // MIPS with FPU
#define IMAGE_FILE_MACHINE_MIPSFPU16 0x0466  // MIPS16 with FPU
#define IMAGE_FILE_MACHINE_POWERPC   0x01f0  // Power PC little endian
#define IMAGE_FILE_MACHINE_POWERPCFP 0x01f1  // Power PC with floating point support
#define IMAGE_FILE_MACHINE_R4000     0x0166  // MIPS little endian
#define IMAGE_FILE_MACHINE_SH3       0x01a2  // Hitachi SH3
#define IMAGE_FILE_MACHINE_SH3DSP    0x01a3  // Hitachi SH3 DSP
#define IMAGE_FILE_MACHINE_SH4       0x01a6  // Hitachi SH4
#define IMAGE_FILE_MACHINE_SH5       0x01a8  // Hitachi SH5
#define IMAGE_FILE_MACHINE_THUMB     0x01c2  // ARM or Thumb( "interworking" )
#define IMAGE_FILE_MACHINE_WCEMIPSV2 0x0169  // MIPS little - endian WCE v2

#define IMAGE_SCN_TYPE_NO_PAD            0x00000008 // The section should not be padded to the next boundary.This flag is obsolete and is replaced by IMAGE_SCN_ALIGN_1BYTES.This is valid only for object files.
#define IMAGE_SCN_CNT_CODE               0x00000020 // The section contains executable code.
#define IMAGE_SCN_CNT_INITIALIZED_DATA   0x00000040 // The section contains initialized data.
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA 0x00000080 // The section contains uninitialized data.
#define IMAGE_SCN_LNK_OTHER              0x00000100 // Reserved for future use.
#define IMAGE_SCN_LNK_INFO               0x00000200 // The section contains comments or other information.The.drectve section has this type.This is valid for object files only.
#define IMAGE_SCN_LNK_REMOVE             0x00000800 // The section will not become part of the image.This is valid only for object files.
#define IMAGE_SCN_LNK_COMDAT             0x00001000 // The section contains COMDAT data.For more information, see section 5.5.6, "COMDAT Sections( Object Only )." This is valid only for object files.
#define IMAGE_SCN_GPREL                  0x00008000 // The section contains data referenced through the global pointer( GP ).
#define IMAGE_SCN_MEM_PURGEABLE          0x00020000 // Reserved for future use.
#define IMAGE_SCN_MEM_16BIT              0x00020000 // For ARM machine types, the section contains Thumb code.Reserved for future use with other machine types.
#define IMAGE_SCN_MEM_LOCKED             0x00040000 // Reserved for future use.
#define IMAGE_SCN_MEM_PRELOAD            0x00080000 // Reserved for future use.
#define IMAGE_SCN_ALIGN_1BYTES           0x00100000 // Align data on a 1 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_2BYTES           0x00200000 // Align data on a 2 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_4BYTES           0x00300000 // Align data on a 4 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_8BYTES           0x00400000 // Align data on an 8 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_16BYTES          0x00500000 // Align data on a 16 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_32BYTES          0x00600000 // Align data on a 32 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_64BYTES          0x00700000 // Align data on a 64 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_128BYTES         0x00800000 // Align data on a 128 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_256BYTES         0x00900000 // Align data on a 256 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_512BYTES         0x00A00000 // Align data on a 512 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_1024BYTES        0x00B00000 // Align data on a 1024 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_2048BYTES        0x00C00000 // Align data on a 2048 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_4096BYTES        0x00D00000 // Align data on a 4096 - byte boundary.Valid only for object files.
#define IMAGE_SCN_ALIGN_8192BYTES        0x00E00000 // Align data on an 8192 - byte boundary.Valid only for object files.
#define IMAGE_SCN_LNK_NRELOC_OVFL        0x01000000 // The section contains extended relocations.
#define IMAGE_SCN_MEM_DISCARDABLE        0x02000000 // The section can be discarded as needed.
#define IMAGE_SCN_MEM_NOT_CACHED         0x04000000 // The section cannot be cached.
#define IMAGE_SCN_MEM_NOT_PAGED          0x08000000 // The section is not pageable.
#define IMAGE_SCN_MEM_SHARED             0x10000000 // The section can be shared in memory.
#define IMAGE_SCN_MEM_EXECUTE            0x20000000 // The section can be executed as code.
#define IMAGE_SCN_MEM_READ               0x40000000 // The section can be read.
#define IMAGE_SCN_MEM_WRITE              0x80000000 // The section can be written to.

#define COFFI_NAME_SIZE 8

    //------------------------------------------------------------------------------
    struct msdos_header
    {
        uint16_t signature;            // 0x4d, 0x5a. This is the "magic number"
        // of an EXE file
        uint16_t bytes_in_last_block;  // The number of bytes in the last block
        // of the program that are actually used.
        // If this value is zero, that means the
        // entire last block is used
        // (i.e. the effective value is 512)
        uint16_t blocks_in_file;       // Number of blocks in the file that are
        // part of the EXE file. If [02-03] is
        // non-zero, only that much of the last
        // block is used
        uint16_t num_relocs;           // Number of relocation entries stored after
        // the header. May be zero
        uint16_t header_paragraphs;    // Number of paragraphs in the header.
        // The program's data begins just after the
        // header, and this field can be used to
        // calculate the appropriate file offset.
        // The header includes the relocation entries.
        // Note that some OSs and/or programs may
        // fail if the header is not a multiple
        // of 512 bytes.
        uint16_t min_extra_paragraphs; // Number of paragraphs of additional memory
        // that the program will need. This is the
        // equivalent of the BSS size in a Unix program. 
        // The program can't be loaded if there 
        // isn't at least this much memory 
        // available to it
        uint16_t max_extra_paragraphs; // Maximum number of paragraphs of additional
        // memory. Normally, the OS reserves all
        // the remaining conventional memory for
        // your program, but you can limit it with
        // this field
        uint16_t ss;                   // Relative value of the stack segment.
        // This value is added to the segment the
        // program was loaded at, and the result
        // is used to initialize the SS register
        uint16_t sp;                   // Initial value of the SP register
        uint16_t checksum;             // Word checksum. If set properly, the
        // 16-bit sum of all words in the file
        // should be zero. Usually, this isn't 
        // filled in
        uint16_t ip;                   // Initial value of the IP register (entry point)
        uint16_t cs;                   // Initial value of the CS register, relative
        // to the segment the program was loaded at
        uint16_t reloc_table_offset;   // Offset of the first relocation item
        // in the file
        uint16_t overlay_number;       // Overlay number. Normally zero, meaning
        // that it's the main program
        short reserved1[4];
        short oem_id;
        short oem_info;
        short reserved2[10];
        long  pe_sign_location;        // Location of PE format signature
    } __attribute__((packed));

    //------------------------------------------------------------------------------
    struct msdos_header_rel
    {
        uint16_t offset;
        uint16_t segment;
    } __attribute__((packed));

    //------------------------------------------------------------------------------
    struct coff_file_header
    {
        uint16_t machine;                 // Identifies the type of target machine
        uint16_t sections_count;          // The size of the section table
        uint32_t time_data_stamp;         // A C run-time time_t value
        uint32_t symbol_table_offset;     // The file offset of the COFF symbol table,
        // or zero if no COFF symbol table is present
        uint32_t symbols_count;           // The number of entries in the symbol table
        uint16_t optional_header_size;    // Which is required for executable files
        // but not for object files.
        uint16_t flags;                   // The flags that indicate the attributes of the file
    } __attribute__((packed));

    //------------------------------------------------------------------------------
    struct common_optional_header
    {
        uint16_t magic;                   // State of the image file identifier
        uint8_t  major_linker_version;    // The linker major version number
        uint8_t  minor_linker_version;    // The linker minor version number
        uint32_t code_size;               // The sum of all code sections 
        uint32_t initialized_data_size;   // The sum of all initialized data sections
        uint32_t uninitialized_data_size; // The sum of all uninitialized data sections
        uint32_t entry_point_address;     // The address of the entry point relative to the image base 
        uint32_t code_base;               // The address that is relative to the image
        // base of the beginning-of-code section 
        uint32_t data_base;               // For PE32 only
    } __attribute__((packed));

    //------------------------------------------------------------------------------
    struct win_headerPE
    {
        uint32_t image_base;
        uint32_t section_alignment;
        uint32_t file_alignment;
        uint16_t major_os_version;
        uint16_t minor_os_version;
        uint16_t major_image_version;
        uint16_t minor_image_version;
        uint16_t major_subsystem_version;
        uint16_t minor_subsystem_version;
        uint32_t win32_version_value;
        uint32_t image_size;
        uint32_t headers_size;
        uint32_t checksum;
        uint16_t subsystem;
        uint16_t dll_flags;
        uint32_t stack_reserve_size;
        uint32_t stack_commit_size;
        uint32_t heap_reserve_size;
        uint32_t heap_commit_size;
        uint32_t loader_flags;
        uint32_t number_of_rva_and_sizes;
    } __attribute__((packed));

    //------------------------------------------------------------------------------
    struct win_headerPEPlus
    {
        uint64_t image_base;
        uint32_t section_alignment;
        uint32_t file_alignment;
        uint16_t major_os_version;
        uint16_t minor_os_version;
        uint16_t major_image_version;
        uint16_t minor_image_version;
        uint16_t major_subsystem_version;
        uint16_t minor_subsystem_version;
        uint32_t win32_version_value;
        uint32_t image_size;
        uint32_t headers_size;
        uint32_t checksum;
        uint16_t subsystem;
        uint16_t dll_flags;
        uint64_t stack_reserve_size;
        uint64_t stack_commit_size;
        uint64_t heap_reserve_size;
        uint64_t heap_commit_size;
        uint32_t loader_flags;
        uint32_t number_of_rva_and_sizes;
    } __attribute__((packed));

    //------------------------------------------------------------------------------
    struct symbol
    {
        char     name[8];
        uint32_t value;
        uint16_t section_number;
        uint16_t type;
        uint8_t  storage_class;
        uint8_t  aux_symbols_number;
        symbol(): name{0} {}
    } __attribute__((packed));

    struct auxiliary_symbol_record
    {
        char value[sizeof(symbol)];
    };

    struct symbol_ext
    {
        symbol sym;
        std::vector<auxiliary_symbol_record> auxs;
    };

    //------------------------------------------------------------------------------
    struct image_data_directory
    {
        uint32_t virtual_address;
        uint32_t size;
    };

    //------------------------------------------------------------------------------
    struct section_header
    {
        char    name[8];
        uint32_t virtual_size;
        uint32_t virtual_address;
        uint32_t data_size;
        uint32_t data_offset;
        uint32_t reloc_offset;
        uint32_t line_num_offset;
        uint16_t reloc_count;
        uint16_t line_num_count;
        uint32_t flags;
    };

    //------------------------------------------------------------------------------
    class string_to_name_provider
    {
    public:
        virtual std::string string_to_name( const char* str ) = 0;
    };

    //------------------------------------------------------------------------------
    class symbol_provider
    {
    public:
        virtual const symbol_ext get_symbol( uint32_t index ) = 0;
    };

    //------------------------------------------------------------------------------
    struct rel_entry
    {
        uint32_t virtual_address;
        uint32_t symbol_table_index;
        uint16_t type;
    };
}

#endif // COFFI_TYPES_HPP
