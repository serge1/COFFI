#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#define BOOST_TEST_MAIN
#ifndef _MSC_VER
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE COFFIO_test

#include <boost/test/unit_test.hpp>

#include <coffi/coffi.hpp>

using namespace COFFI;

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( load_msdos_header )
{
    coffi c;

    BOOST_CHECK_EQUAL( c.load( "data/notep.exe" ), false );
    BOOST_CHECK_EQUAL( c.load( "data/notepad.exe" ), true );
    BOOST_CHECK_EQUAL( c.load( "data/notepad-corrupted1.exe" ), false );
    BOOST_CHECK_EQUAL( c.load( "data/notepad-corrupted2.exe" ), false );
    BOOST_CHECK_EQUAL( c.load( "data/notepad-corrupted3.exe" ), false );
    BOOST_CHECK_EQUAL( c.load( "data/notepad-corrupted4.exe" ), false );
    BOOST_CHECK_EQUAL( c.load( "data/espui.dll" ), true );
    BOOST_CHECK_NE( c.get_msdos_header(), (dos_header*)0 );
    BOOST_CHECK_EQUAL( c.load( "data/anobj.o" ), true );
    BOOST_CHECK_EQUAL( c.get_msdos_header(), (dos_header*)0 );

    coffi c1;

    BOOST_CHECK_EQUAL( c1.load( "data/notep.exe" ), false );
    BOOST_CHECK_EQUAL( c1.load( "data/notepad.exe" ), true );
    BOOST_CHECK_EQUAL( c1.load( "data/notepad-corrupted1.exe" ), false );
    BOOST_CHECK_EQUAL( c1.load( "data/notepad-corrupted2.exe" ), false );
    BOOST_CHECK_EQUAL( c1.load( "data/notepad-corrupted3.exe" ), false );
    BOOST_CHECK_EQUAL( c1.load( "data/notepad-corrupted4.exe" ), false );
    BOOST_CHECK_EQUAL( c1.load( "data/espui.dll" ), true );
    BOOST_CHECK_NE( c1.get_msdos_header(), (dos_header*)0 );
    BOOST_CHECK_EQUAL( c1.load( "data/anobj.o" ), true );
    BOOST_CHECK_EQUAL( c1.get_msdos_header(), (dos_header*)0 );

    BOOST_REQUIRE_NE( c.get_header(), (void*)0 );
    BOOST_REQUIRE_NE( c1.get_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_header()->get_machine(), c1.get_header()->get_machine() );
}


//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( load_coff_header )
{
    coffi c;

    BOOST_REQUIRE_EQUAL( c.load( "data/notepad.exe" ), true );
    BOOST_REQUIRE_NE( c.get_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_header()->get_machine(), IMAGE_FILE_MACHINE_AMD64 );
    BOOST_REQUIRE_NE( c.get_opt_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_magic(), OH_MAGIC_PE32PLUS );
}


//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( load_coff_header_dll )
{
    coffi c;

    BOOST_REQUIRE_EQUAL( c.load( "data/espui.dll" ), true );
    BOOST_REQUIRE_NE( c.get_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_header()->get_machine(), IMAGE_FILE_MACHINE_I386 );
    BOOST_REQUIRE_NE( c.get_opt_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_magic(), OH_MAGIC_PE32 );
}


//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( load_coff_header_obj )
{
    coffi c;

    BOOST_REQUIRE_EQUAL( c.load( "data/coffi_test.ob" ), true );
    BOOST_REQUIRE_NE( c.get_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_header()->get_machine(), IMAGE_FILE_MACHINE_I386 );
    BOOST_CHECK_EQUAL( c.get_opt_header(), (void*)0 );
}


//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( load_sections_obj )
{
    coffi c;

    BOOST_REQUIRE_EQUAL( c.load( "data/coffi_test.ob" ), true );
    BOOST_REQUIRE_NE( c.get_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_header()->get_machine(), IMAGE_FILE_MACHINE_I386 );
    BOOST_CHECK_EQUAL( c.get_opt_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_sections().size(), c.get_header()->get_sections_count() );
    for ( int i = 0; i < c.get_header()->get_sections_count(); i++ ) {
        BOOST_CHECK_EQUAL( c.get_sections()[i]->get_index(), i );
    }
    BOOST_CHECK_EQUAL( c.get_sections()[0]->get_name(), ".drectve" );
    BOOST_CHECK_EQUAL( c.get_sections()[1]->get_name(), ".debug$S" );
    BOOST_CHECK_EQUAL( c.get_sections()[c.get_header()->get_sections_count() - 1]->get_name(), ".debug$S" );
}


//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( load_sections_exe )
{
    coffi c;

    BOOST_REQUIRE_EQUAL( c.load( "data/notepad.exe" ), true );
    BOOST_REQUIRE_NE( c.get_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_header()->get_machine(), IMAGE_FILE_MACHINE_AMD64 );
    BOOST_CHECK_NE( c.get_opt_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_sections().size(), c.get_header()->get_sections_count() );
    for ( int i = 0; i < c.get_header()->get_sections_count(); i++ ) {
        BOOST_CHECK_EQUAL( c.get_sections()[i]->get_index(), i );
    }

    BOOST_CHECK_EQUAL( c.get_directory()[1].virtual_address, 0xCFF8 );
    BOOST_CHECK_EQUAL( c.get_directory()[1].size, 300 );
    BOOST_CHECK_EQUAL( c.get_directory()[2].virtual_address, 0x14000 );
    BOOST_CHECK_EQUAL( c.get_directory()[2].size, 127328 );
    BOOST_CHECK_EQUAL( c.get_directory()[3].virtual_address, 0x13000 );
    BOOST_CHECK_EQUAL( c.get_directory()[3].size, 1716 );
    BOOST_CHECK_EQUAL( c.get_directory()[11].virtual_address, 0x2E0 );
    BOOST_CHECK_EQUAL( c.get_directory()[11].size, 312 );
    BOOST_CHECK_EQUAL( c.get_directory()[12].virtual_address, 0xC000 );
    BOOST_CHECK_EQUAL( c.get_directory()[12].size, 2032 );

    BOOST_CHECK_EQUAL( c.get_sections()[0]->get_name(), ".text" );
    BOOST_CHECK_EQUAL( c.get_sections()[1]->get_name(), ".rdata" );
    BOOST_CHECK_EQUAL( c.get_sections()[2]->get_name(), ".data" );
    BOOST_CHECK_EQUAL( c.get_sections()[3]->get_name(), ".pdata" );
    BOOST_CHECK_EQUAL( c.get_sections()[4]->get_name(), ".rsrc" );
    BOOST_CHECK_EQUAL( c.get_sections()[5]->get_name(), ".reloc" );
}


//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( load_sections_a )
{
    coffi c;

    BOOST_REQUIRE_EQUAL( c.load( "data/demo.a" ), true );
    BOOST_REQUIRE_NE( c.get_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_header()->get_machine(), 0xdca6 );
    BOOST_CHECK_EQUAL( c.get_header()->get_sections_count(), 145 );
    BOOST_CHECK_EQUAL( c.get_header()->get_symbol_table_offset(), 53935 );
    BOOST_CHECK_EQUAL( c.get_header()->get_symbols_count(), 977 );
    BOOST_CHECK_EQUAL( c.get_header()->get_flags(), 0x0304 );

    BOOST_CHECK_NE( c.get_opt_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_sections().size(), c.get_header()->get_sections_count() );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_magic(), 0xa00 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_code_base(), 0 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_code_size(), 0 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_data_base(), 0 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_initialized_data_size(), 0 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_entry_point_address(), 0 );

    for ( int i = 0; i < c.get_header()->get_sections_count(); i++ ) {
        BOOST_CHECK_EQUAL( c.get_sections()[i]->get_index(), i );
        BOOST_CHECK_EQUAL( c.get_sections()[i]->get_relocations().size(), c.get_sections()[i]->get_reloc_count() );
    }
    BOOST_CHECK_EQUAL( c.get_sections()[0]->get_name(), "CODE@c0" );
    BOOST_CHECK_EQUAL( c.get_sections()[1]->get_name(), "const_data@d0" );
    BOOST_CHECK_EQUAL( c.get_sections()[2]->get_name(), ".bss@d0" );
    BOOST_CHECK_EQUAL( c.get_sections()[3]->get_name(), ".data@d0" );
    BOOST_CHECK_EQUAL( c.get_sections()[4]->get_name(), "inttbl@c0" );
    BOOST_CHECK_EQUAL( c.get_sections()[5]->get_name(), "inttbl@c0" );

    BOOST_CHECK_EQUAL( c.get_sections()[1]->get_relocations()[0].get_virtual_address(), 0 );
    BOOST_CHECK_EQUAL( c.get_sections()[1]->get_relocations()[0].get_symbol(), std::string( "Lt_0_65" ) );
    //BOOST_CHECK_EQUAL( c.get_sections()[1]->get_relocations()[0].get_type(), 50001 );

    BOOST_CHECK_EQUAL( c.get_sections()[1]->get_relocations()[3].get_virtual_address(), 0xC );
    BOOST_CHECK_EQUAL( c.get_sections()[1]->get_relocations()[3].get_symbol(), std::string( "Lt_0_14" ) );
    //BOOST_CHECK_EQUAL( c.get_sections()[1]->get_relocations()[3].get_type(), 50001 );

    BOOST_CHECK_EQUAL( c.get_sections()[55]->get_relocations()[2].get_virtual_address(), 0x45 );
    BOOST_CHECK_EQUAL( c.get_sections()[55]->get_relocations()[2].get_symbol(), std::string( "_current_malloc" ) );
    //BOOST_CHECK_EQUAL( c.get_sections()[55]->get_relocations()[2].get_type(), 100381 );

    BOOST_CHECK_EQUAL( c.get_sections()[73]->get_relocations()[3].get_virtual_address(), 0x79 );
    BOOST_CHECK_EQUAL( c.get_sections()[73]->get_relocations()[3].get_symbol(), std::string( "BB86_readMailboxAll" ) );
    //BOOST_CHECK_EQUAL( c.get_sections()[73]->get_relocations()[3].get_type(), 4540782 );
}


//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( load_sections_o )
{
    coffi c;

    BOOST_REQUIRE_EQUAL( c.load( "data/demo.o" ), true );
    BOOST_REQUIRE_NE( c.get_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_header()->get_machine(), 0x8ca6 );
    BOOST_CHECK_EQUAL( c.get_header()->get_sections_count(), 7 );
    BOOST_CHECK_EQUAL( c.get_header()->get_symbol_table_offset(), 642 );
    BOOST_CHECK_EQUAL( c.get_header()->get_symbols_count(), 57 );
    BOOST_CHECK_EQUAL( c.get_header()->get_flags(), 0x0304 );

    BOOST_CHECK_NE( c.get_opt_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_sections().size(), c.get_header()->get_sections_count() );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_magic(), 0xa00 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_code_base(), 0 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_code_size(), 0 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_data_base(), 0 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_initialized_data_size(), 0 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_entry_point_address(), 0 );

    for ( int i = 0; i < c.get_header()->get_sections_count(); i++ ) {
        BOOST_CHECK_EQUAL( c.get_sections()[i]->get_index(), i );
    }
    BOOST_CHECK_EQUAL( c.get_sections()[0]->get_name(), "CODE" );
    BOOST_CHECK_EQUAL( c.get_sections()[1]->get_name(), "const_data" );
    BOOST_CHECK_EQUAL( c.get_sections()[2]->get_name(), ".bss" );
    BOOST_CHECK_EQUAL( c.get_sections()[3]->get_name(), ".data" );
    BOOST_CHECK_EQUAL( c.get_sections()[4]->get_name(), ".text" );
    BOOST_CHECK_EQUAL( c.get_sections()[5]->get_name(), ".text" );
    BOOST_CHECK_EQUAL( c.get_sections()[6]->get_name(), ".text" );
}


//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( load_section_data )
{
    coffi c;

    BOOST_REQUIRE_EQUAL( c.load( "data/notepad.exe" ), true );

    BOOST_REQUIRE_NE( c.get_sections()[".text"], (section*)0 );
    BOOST_CHECK_EQUAL( c.get_sections()[".text"]->get_name(), ".text" );
    const unsigned char* data = (const unsigned char*)c.get_sections()[".text"]->get_data();
    const unsigned char real[] = {
        0x66, 0x83, 0x3b, 0x09, 0x0f, 0x84, 0x5a, 0x25,
        0x00, 0x00, 0x44, 0x8b, 0xc6, 0x48, 0x8b, 0xd3};
    BOOST_CHECK_EQUAL_COLLECTIONS( data, data + sizeof( real ), real, real + sizeof( real ) );

    const unsigned char real1[] = {
        0xb3, 0xc9, 0x5b, 0x4a, 0x35, 0x02, 0x7e, 0x19,
        0x0a, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x48, 0xb7, 0x00, 0x00, 0x48, 0xad, 0x00, 0x00,
        0x7e, 0x19, 0x03, 0xbb, 0x52, 0x53, 0x44, 0x53};
    BOOST_CHECK_EQUAL_COLLECTIONS( data + 0xA730, data + 0xA730 + sizeof( real1 ), real1, real1 + sizeof( real1 ) );

    BOOST_REQUIRE_NE( c.get_sections()[".pdata"], (section*)0 );
    BOOST_CHECK_EQUAL( c.get_sections()[".pdata"]->get_name(), ".pdata" );
    data = (const unsigned char*)c.get_sections()[".pdata"]->get_data();
    const unsigned char real3[] = {
        0x00, 0x10, 0x00, 0x00, 0xc8, 0x10, 0x00, 0x00,
        0x35, 0x32, 0x01, 0x00, 0xc8, 0x10, 0x00, 0x00};
    BOOST_CHECK_EQUAL_COLLECTIONS( data, data + sizeof( real3 ), real3, real3 + sizeof( real3 ) );

    const unsigned char real4[] = {
        0x3c, 0xb6, 0x00, 0x00, 0xd4, 0xb6, 0x00, 0x00,
        0x38, 0xf1, 0x00, 0x00, 0xd4, 0xb6, 0x00, 0x00,
        0xf0, 0xb6, 0x00, 0x00, 0x58, 0xf1, 0x00, 0x00,
        0xf0, 0xb6, 0x00, 0x00, 0x10, 0xb7, 0x00, 0x00};
    BOOST_CHECK_EQUAL_COLLECTIONS( data + 0x690, data + 0x690 + sizeof( real4 ), real4, real4 + sizeof( real4 ) );
}


//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( load_section_data1 )
{
    coffi c;

    BOOST_REQUIRE_EQUAL( c.load( "data/demo.a" ), true );

    BOOST_REQUIRE_NE( c.get_sections()[1], (section*)0 );
    BOOST_REQUIRE_NE( c.get_sections()["const_data@d0"], (section*)0 );
    BOOST_CHECK_EQUAL( c.get_sections()["const_data@d0"]->get_name(), "const_data@d0" );
    BOOST_CHECK_EQUAL( c.get_sections()[1]->get_name(), "const_data@d0" );
    BOOST_CHECK_EQUAL( c.get_sections()[1]->get_data_size(), 104 );
    const unsigned char* data = (const unsigned char*)c.get_sections()["const_data@d0"]->get_data();
    const unsigned char real[] = {
        0xa6, 0x20, 0x00, 0x00, 0xa6, 0x20, 0x00, 0x00,
        0x80, 0x23, 0x00, 0x00, 0x60, 0x23, 0x00, 0x00};
    BOOST_CHECK_EQUAL_COLLECTIONS( data, data + sizeof( real ), real, real + sizeof( real ) );

    BOOST_REQUIRE_NE( c.get_sections()[16], (section*)0 );
    BOOST_CHECK_EQUAL( c.get_sections()[16]->get_name(), "inttbl@c0" );
    BOOST_CHECK_EQUAL( c.get_sections()[16]->get_data_size(), 8 );
    data = (const unsigned char*)c.get_sections()[16]->get_data();
    const unsigned char real1[] = {
        0xf8, 0x00, 0x00, 0x00, 0x9a, 0xa0, 0x46, 0x1f};
    BOOST_CHECK_EQUAL_COLLECTIONS( data, data + sizeof( real1 ), real1, real1 + sizeof( real1 ) );

    BOOST_REQUIRE_NE( c.get_sections()[50], (section*)0 );
    BOOST_CHECK_EQUAL( c.get_sections()[50]->get_name(), "setFlags_section@c0" );
    BOOST_CHECK_EQUAL( c.get_sections()[50]->get_data_size(), 178 );
    data = (const unsigned char*)c.get_sections()[50]->get_data();
    const unsigned char real2[] = {
        0x3d, 0x10, 0x06, 0x20, 0x5c, 0xbc, 0x16, 0xc0};
    BOOST_CHECK_EQUAL_COLLECTIONS( data, data + sizeof( real2 ), real2, real2 + sizeof( real2 ) );

    BOOST_REQUIRE_NE( c.get_sections()[74], (section*)0 );
    BOOST_CHECK_EQUAL( c.get_sections()[74]->get_name(), "doWrite_section@c0" );
    BOOST_CHECK_EQUAL( c.get_sections()[74]->get_data_size(), 286 );
    data = (const unsigned char*)c.get_sections()[74]->get_data();
    const unsigned char real3[] = {
        0x4f, 0x63, 0x3c, 0xa0, 0x8d, 0xc4, 0x16, 0x07};
    BOOST_CHECK_EQUAL_COLLECTIONS( data + 278, data + 278 + sizeof( real3 ), real3, real3 + sizeof( real3 ) );
}


//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( load_sections_tclsh_exe )
{
    coffi c;

    BOOST_REQUIRE_EQUAL( c.load( "data/tclsh.exe" ), true );
    BOOST_REQUIRE_NE( c.get_header(), (void*)0 );
    BOOST_CHECK_EQUAL( c.get_header()->get_machine(), IMAGE_FILE_MACHINE_I386 );
    BOOST_CHECK_EQUAL( c.get_header()->get_sections_count(), 6 );
    BOOST_CHECK_EQUAL( c.get_header()->get_optional_header_size(), 0xE0 );

    BOOST_CHECK_EQUAL( c.get_sections().size(), c.get_header()->get_sections_count() );
    for ( int i = 0; i < c.get_header()->get_sections_count(); i++ ) {
        BOOST_CHECK_EQUAL( c.get_sections()[i]->get_index(), i );
    }

    BOOST_REQUIRE_NE( c.get_opt_header(), (optional_header*)0x0 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_magic(), 0x010B );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_initialized_data_size(), 0x2600 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_uninitialized_data_size(), 0x200 );
    BOOST_CHECK_EQUAL( c.get_opt_header()->get_entry_point_address(), 0x1280 );

    BOOST_REQUIRE_NE( c.get_win_header(), (const win_header*)0x0 );
    BOOST_CHECK_EQUAL( c.get_win_header()->get_image_base(), 0x400000 );
    BOOST_CHECK_EQUAL( c.get_win_header()->get_headers_size(), 0x400 );
    BOOST_CHECK_EQUAL( c.get_win_header()->get_heap_commit_size(), 0x1000 );
    BOOST_CHECK_EQUAL( c.get_win_header()->get_number_of_rva_and_sizes(), 16 );

    BOOST_CHECK_EQUAL( c.get_directory().size(), 16 );
    BOOST_CHECK_EQUAL( c.get_directory()[1].virtual_address, 0x5000 );
    BOOST_CHECK_EQUAL( c.get_directory()[1].size, 0x358 );
    BOOST_CHECK_EQUAL( c.get_directory()[2].virtual_address, 0x6000 );
    BOOST_CHECK_EQUAL( c.get_directory()[2].size, 0x129C );
    BOOST_CHECK_EQUAL( c.get_directory()[14].virtual_address, 0x0 );
    BOOST_CHECK_EQUAL( c.get_directory()[14].size, 0x0 );
    BOOST_CHECK_EQUAL( c.get_directory()[15].virtual_address, 0x0 );
    BOOST_CHECK_EQUAL( c.get_directory()[15].size, 0x0 );

    BOOST_CHECK_EQUAL( c.get_sections()[0]->get_name(), ".text" );
    BOOST_CHECK_EQUAL( c.get_sections()[1]->get_name(), ".data" );
    BOOST_CHECK_EQUAL( c.get_sections()[2]->get_name(), ".rdata" );
    BOOST_CHECK_EQUAL( c.get_sections()[3]->get_name(), ".bss" );
    BOOST_CHECK_EQUAL( c.get_sections()[4]->get_name(), ".idata" );
    BOOST_CHECK_EQUAL( c.get_sections()[5]->get_name(), ".rsrc" );
}
