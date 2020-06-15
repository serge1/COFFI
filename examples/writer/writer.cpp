#include <coffi/coffi.hpp>

using namespace COFFI;

void write_the_file( const std::string &filename )
{
    coffi writer;
    
    // You can't proceed without this function call!
    writer.create(COFFI_ARCHITECTURE_PE);

    // Create the optional header (required for images *.exe, *.dll)
    writer.create_optional_header();

    // Create code section
    section* text_sec = writer.add_section( ".text" );
    // Code that returns 42
    char text[] = {
        '\x55',                                  // push   ebp
        '\x89', '\xE5',                          // mov    ebp,esp
        '\xB8', '\x2A', '\x00', '\x00', '\x00',  // mov    eax,0x2a
        '\x5D',                                  // pop    ebp
        '\xC3',                                  // ret
        '\x90',                                  // nop
        '\x90',                                  // nop
        '\xFF', '\xFF', '\xFF', '\xFF',          // Unused bytes
        '\x00', '\x00', '\x00', '\x00',          // Unused bytes
        '\xFF', '\xFF', '\xFF', '\xFF',          // Unused bytes
        '\x00', '\x00', '\x00', '\x00',          // Unused bytes
    };
    text_sec->set_data( text, sizeof( text ) );
    text_sec->set_virtual_address(0x1000);
    text_sec->set_virtual_size(sizeof(text));
    text_sec->set_flags(IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_CODE | IMAGE_SCN_ALIGN_4BYTES);

    // Create a .rdata section, with arbitrary data
    section* rdata_sec = writer.add_section( ".rdata" );
    char rdata[] = "This is an arbitrary read-only string data\0";
    rdata_sec->set_data( rdata, sizeof( rdata ) );
    rdata_sec->set_virtual_address(0x2000);
    rdata_sec->set_virtual_size(sizeof(rdata));
    rdata_sec->set_flags(IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_4BYTES);

    // Create a .idata section, empty
    section* idata_sec = writer.add_section( ".idata" );
    char idata[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    idata_sec->set_data( idata, sizeof( idata ) );
    idata_sec->set_virtual_address(0x3000);
    idata_sec->set_virtual_size(sizeof(idata));
    idata_sec->set_flags(IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_4BYTES);

    // Set headers properties
    //writer.get_header()->set_flags(0x102); // EXEC_P, D_PAGED
    writer.get_header()->set_flags(IMAGE_FILE_RELOCS_STRIPPED | IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_LINE_NUMS_STRIPPED | IMAGE_FILE_LOCAL_SYMS_STRIPPED | IMAGE_FILE_32BIT_MACHINE | IMAGE_FILE_DEBUG_STRIPPED);
    // Characteristics : relocations stripped, executable, line numbers stripped, symbols stripped, 32 bit words, debugging information removed
    writer.get_optional_header()->set_code_size(0x200);
    writer.get_optional_header()->set_initialized_data_size(0x400);
    writer.get_optional_header()->set_entry_point_address(0x00001000);
    writer.get_optional_header()->set_code_base(0x00001000);
    writer.get_win_header()->set_image_base(0x00400000);
    writer.get_win_header()->set_section_alignment(0x1000);
    writer.get_win_header()->set_major_os_version(4);
    writer.get_win_header()->set_major_image_version(1);
    writer.get_win_header()->set_major_subsystem_version(4);
    writer.get_win_header()->set_image_size(0x4000);
    writer.get_win_header()->set_headers_size(0x200);
    writer.get_win_header()->set_subsystem(3); // Windows CUI
    writer.get_win_header()->set_stack_reserve_size(0x00200000);
    writer.get_win_header()->set_stack_commit_size(0x1000);
    writer.get_win_header()->set_heap_reserve_size(0x100000);
    writer.get_win_header()->set_heap_commit_size(0x1000);

    // Add directories (required for images *.exe, *.dll)
    writer.add_directory(image_data_directory{0, 0}); // Export Directory [.edata (or where ever we found it)]
    writer.add_directory(image_data_directory{idata_sec->get_virtual_address(), idata_sec->get_virtual_size()}); // Import Directory [parts of .idata]
    writer.add_directory(image_data_directory{0, 0}); // Resource Directory [.rsrc]
    writer.add_directory(image_data_directory{0, 0}); // Exception Directory [.pdata]
    writer.add_directory(image_data_directory{0, 0}); // Security Directory
    writer.add_directory(image_data_directory{0, 0}); // Base Relocation Directory [.reloc]
    writer.add_directory(image_data_directory{0, 0}); // Debug Directory
    writer.add_directory(image_data_directory{0, 0}); // Description Directory
    writer.add_directory(image_data_directory{0, 0}); // Special Directory
    writer.add_directory(image_data_directory{0, 0}); // Thread Storage Directory [.tls]
    writer.add_directory(image_data_directory{0, 0}); // Load Configuration Directory
    writer.add_directory(image_data_directory{0, 0}); // Bound Import Directory
    writer.add_directory(image_data_directory{0, 0}); // Import Address Table Directory
    writer.add_directory(image_data_directory{0, 0}); // Delay Import Directory
    writer.add_directory(image_data_directory{0, 0}); // CLR Runtime Header
    writer.add_directory(image_data_directory{0, 0}); // Reserved

    // Create the PE file
    writer.save( filename );
}

int main()
{
    write_the_file( "answer.exe" );
    return 0;
}
