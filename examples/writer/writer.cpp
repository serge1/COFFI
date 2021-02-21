/* This example shows how to create a PE program file for Windows
 *
 * Instructions:
 * 1. Compile and link this file with the COFFI library
 *    g++ writer.cpp -o writer_obj.exe
 * 2. Execute the result file writer.exe
 *    writer.exe
 *    It will produce hello.exe
 * 3. Run the result file:
 *    hello.exe
 *    It should open a "Hello World!" message box
 */

//! [ex_writer_open]
#include <coffi/coffi.hpp>

using namespace COFFI;

// clang-format off

void write_the_file( const std::string &filename )
{
    coffi writer;
    
    // You can't proceed without this function call!
    writer.create(COFFI_ARCHITECTURE_PE);
    //! [ex_writer_open]

    //! [ex_writer_optional]
    // Create the optional header (required for images *.exe, *.dll)
    writer.create_optional_header();
    //! [ex_writer_optional]

    //! [ex_writer_code]
    // Create code section
    section* text_sec = writer.add_section( ".text" );
    char text[] = {
        '\x6A', '\x00',                                 // push 0
        '\x68', '\x00', '\x20', '\x40', '\x00',         // push offset 0x00402000 (string in the .rdata section)
        '\x68', '\x00', '\x20', '\x40', '\x00',         // push offset 0x00402000 (string in the .rdata section)
        '\x6A', '\x00',                                 // push 0
        '\xE8', '\x0E', '\x00', '\x00', '\x00',         // call 0x401021
        '\x6A', '\x00',                                 // push 0
        '\xE8', '\x01', '\x00', '\x00', '\x00',         // call 0x40101b
        '\xF4',                                         // hlt
        '\xFF', '\x25', '\x4C', '\x30', '\x40', '\x00', // 0x40101b: jmp *0x40304C (ExitProcess)
        '\xFF', '\x25', '\x54', '\x30', '\x40', '\x00'  // 0x401021: jmp *0x403054 (MessageBoxA)
    };
    text_sec->set_data( text, sizeof( text ) );
    text_sec->set_virtual_address(0x1000);
    text_sec->set_virtual_size(sizeof(text));
    text_sec->set_flags(IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_CODE | IMAGE_SCN_ALIGN_4BYTES);
    //! [ex_writer_code]

    //! [ex_writer_rdata]
    // Create a .rdata section, with the string
    section* rdata_sec = writer.add_section( ".rdata" );
    char rdata[] = "Hello World!\0";
    rdata_sec->set_data( rdata, sizeof( rdata ) );
    rdata_sec->set_virtual_address(0x2000);
    rdata_sec->set_virtual_size(sizeof(rdata));
    rdata_sec->set_flags(IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_4BYTES);
    //! [ex_writer_rdata]

    //! [ex_writer_idata]
    // Create a .idata section
    section* idata_sec = writer.add_section( ".idata" );
    uint8_t idata[] = {
        // RVA = 0x3000

        // Import table (array of IMAGE_IMPORT_DESCRIPTOR structures)
        0x3C, 0x30, 0x00, 0x00,     // OriginalFirstThunk: offset of the import lookup table
        0x00, 0x00, 0x00, 0x00,     // TimeDateStamp
        0x00, 0x00, 0x00, 0x00,     // ForwarderChain
        0x5C, 0x30, 0x00, 0x00,     // Name: Offset of "KERNEL32.dll"
        0x4C, 0x30, 0x00, 0x00,     // FirstThunk: offset of import address table

        // Import table (array of IMAGE_IMPORT_DESCRIPTOR structures)
        0x44, 0x30, 0x00, 0x00,     // OriginalFirstThunk: offset of the import lookup table
        0x00, 0x00, 0x00, 0x00,     // TimeDateStamp
        0x00, 0x00, 0x00, 0x00,     // ForwarderChain
        0x6C, 0x30, 0x00, 0x00,     // Name: Offset of "USER32.dll"
        0x54, 0x30, 0x00, 0x00,     // FirstThunk: offset of import address table

        // Empty IMAGE_IMPORT_DESCRIPTOR structure (signaling the end of the IMAGE_IMPORT_DESCRIPTOR array)
        0x00, 0x00, 0x00, 0x00,     // OriginalFirstThunk
        0x00, 0x00, 0x00, 0x00,     // TimeDateStamp
        0x00, 0x00, 0x00, 0x00,     // ForwarderChain
        0x00, 0x00, 0x00, 0x00,     // Name
        0x00, 0x00, 0x00, 0x00,     // FirstThunk

        // Import lookup table (array of IMAGE_THUNK_DATA structures)
        // RVA = 0x303C
        0x7C, 0x30, 0x00, 0x00,     // Import function by name
        0x00, 0x00, 0x00, 0x00,     //
        // RVA = 0x3044
        0x8C, 0x30, 0x00, 0x00,     // Import function by name
        0x00, 0x00, 0x00, 0x00,     //

        // Import address table (array of IMAGE_THUNK_DATA structures)
        // RVA = 0x304C
        0x7C, 0x30, 0x00, 0x00,     // Import function by name
        0x00, 0x00, 0x00, 0x00,     //
        // RVA = 0x3054
        0x8C, 0x30, 0x00, 0x00,     // Import function by name
        0x00, 0x00, 0x00, 0x00,     //

        // Names
        // RVA = 0x305C
        'K', 'E', 'R', 'N', 'E', 'L', '3', '2', '.', 'd', 'l', 'l', 0, 0, 0, 0,
        // RVA = 0x306C
        'U', 'S', 'E', 'R', '3', '2', '.', 'd', 'l', 'l',  0,   0,  0, 0, 0, 0,

        // Hint/Name Table
        // RVA = 0x307C
        0x5E, 0x01, 'E', 'x', 'i', 't', 'P', 'r', 'o', 'c', 'e', 's', 's', 0, 0, 0,
        // RVA = 0x308C
        0x7F, 0x02, 'M', 'e', 's', 's', 'a', 'g', 'e', 'B', 'o', 'x', 'A', 0, 0, 0,
    };

    idata_sec->set_data( (char*)idata, sizeof( idata ) );
    idata_sec->set_virtual_address(0x3000);
    idata_sec->set_virtual_size(sizeof(idata));
    idata_sec->set_flags(IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_4BYTES);
    //! [ex_writer_idata]

    //! [ex_writer_reloc]
    // Create a .reloc section
    section* reloc_sec = writer.add_section( ".reloc" );
    uint16_t reloc[] = {
        0x1000, 0x0000, // RVA of relocation block
        0x0010, 0x0000, // Size of the relocation block
        0x3003, // Relocation HIGHLOW at address 0x1003
        0x3008, // Relocation HIGHLOW at address 0x1008
        0x301D, // Relocation HIGHLOW at address 0x101D
        0x3023, // Relocation HIGHLOW at address 0x1023
    };
    reloc_sec->set_data( (char*)reloc, sizeof( reloc ) );
    reloc_sec->set_virtual_address(0x4000);
    reloc_sec->set_virtual_size(sizeof(reloc));
    reloc_sec->set_flags(IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_4BYTES);
    //! [ex_writer_reloc]

    //! [ex_writer_header]
    // Set headers properties
    writer.get_header()->set_flags(IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE);
    writer.get_optional_header()->set_code_size(0x200);
    writer.get_optional_header()->set_initialized_data_size(0x400);
    writer.get_optional_header()->set_entry_point_address(0x00001000);
    writer.get_optional_header()->set_code_base(0x00001000);
    writer.get_optional_header()->set_data_base(0x00002000);
    writer.get_win_header()->set_image_base(0x00400000);
    writer.get_win_header()->set_section_alignment(0x1000);
    writer.get_win_header()->set_major_os_version(4);
    writer.get_win_header()->set_major_image_version(1);
    writer.get_win_header()->set_major_subsystem_version(4);
    writer.get_win_header()->set_image_size(0x5000);
    writer.get_win_header()->set_headers_size(0x200);
    writer.get_win_header()->set_subsystem(3); // Windows CUI
    writer.get_win_header()->set_stack_reserve_size(0x00100000);
    writer.get_win_header()->set_stack_commit_size(0x1000);
    writer.get_win_header()->set_heap_reserve_size(0x100000);
    writer.get_win_header()->set_heap_commit_size(0x1000);
    //! [ex_writer_header]

    //! [ex_writer_directories]
    // Add directories (required for images *.exe, *.dll)
    writer.add_directory(image_data_directory{0, 0}); // Export Directory [.edata (or where ever we found it)]
    writer.add_directory(image_data_directory{idata_sec->get_virtual_address(), idata_sec->get_virtual_size()}); // Import Directory [parts of .idata]
    writer.add_directory(image_data_directory{0, 0}); // Resource Directory [.rsrc]
    writer.add_directory(image_data_directory{0, 0}); // Exception Directory [.pdata]
    writer.add_directory(image_data_directory{0, 0}); // Security Directory
    writer.add_directory(image_data_directory{reloc_sec->get_virtual_address(), reloc_sec->get_virtual_size()}); // Base Relocation Directory [.reloc]
    writer.add_directory(image_data_directory{0, 0}); // Debug Directory
    writer.add_directory(image_data_directory{0, 0}); // Description Directory
    writer.add_directory(image_data_directory{0, 0}); // Special Directory
    writer.add_directory(image_data_directory{0, 0}); // Thread Storage Directory [.tls]
    writer.add_directory(image_data_directory{0, 0}); // Load Configuration Directory
    writer.add_directory(image_data_directory{0, 0}); // Bound Import Directory
    writer.add_directory(image_data_directory{0x304C, 0x10}); // Import Address Table Directory
    writer.add_directory(image_data_directory{0, 0}); // Delay Import Directory
    writer.add_directory(image_data_directory{0, 0}); // CLR Runtime Header
    writer.add_directory(image_data_directory{0, 0}); // Reserved
    //! [ex_writer_directories]

    //! [ex_writer_save]
    // Recompute all the offsets in the file
    writer.layout();

    // Create the PE file
    writer.save( filename );
    //! [ex_writer_save]
}
// clang-format on

int main()
{
    write_the_file("hello.exe");
    return 0;
}
