#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include <boost/test/unit_test.hpp>

#include <coffi/coffi.hpp>
#include "coffio_test.hpp"

using namespace COFFI;
using namespace boost::unit_test;

//------------------------------------------------------------------------------

void check_binaries_equal(const std::string &file1, const std::string &file2)
{
    // Read the original file
    std::ifstream binary_file1(file1, std::ios::binary);
    std::string str1((std::istreambuf_iterator<char>(binary_file1)), std::istreambuf_iterator<char>());

    // Read the saved file
    std::ifstream binary_file2(file2, std::ios::binary);
    std::string str2((std::istreambuf_iterator<char>(binary_file2)), std::istreambuf_iterator<char>());

    // Check binaries
    printf("Comparing %s and %s\n", file1.c_str(), file2.c_str());
    BOOST_CHECK_EQUAL(str1 == str2, true);
}

//------------------------------------------------------------------------------
void check_load_save(const std::string &file)
{
    coffi c;

    // Load and save the file
    BOOST_CHECK_EQUAL(c.load(file), true);
    std::string outfile = file + ".tmp";
    c.save(outfile);

    // Run COFFDump on both files
    //RunCOFFDump(file);
    //RunCOFFDump(outfile);
    //
    // Check Dumps
    //check_dumps_equal(dump_test_file_name(file), dump_test_file_name(outfile));

    // Check binaries
    check_binaries_equal(file, outfile);
}

BOOST_AUTO_TEST_CASE(load_save)
{
   check_load_save("data/anobj.o");
   check_load_save("data/coffi_test.obj");
   check_load_save("data/demo.a");
   check_load_save("data/demo.o");
   check_load_save("data/espui.dll");
   check_load_save("data/NikPEViewer.exe");
   // notepad.exe uses non-zero padding bytes, cannot compare the results
   //check_load_save("data/notepad.exe");
   check_load_save("data/tclsh.exe");
   check_load_save("data/ti_c2000_1.obj");
   check_load_save("data/ti_c2000_1.out");
}

//------------------------------------------------------------------------------

void check_append_data(const std::string &file, const std::string &section_name)
{
    coffi c;
    // Load and save the file
    BOOST_CHECK_EQUAL(c.load(file), true);
    // Append same data to a section
    char buffer[1234];
    std::memset(buffer, 0xBE, 1234);
    c.get_sections()[section_name]->append_data(buffer, 1234);
    // Save
    BOOST_CHECK_EQUAL(c.save(file + ".append.tmp"), true);
    // Check
    check_binaries_equal(file + ".append.expected", file + ".append.tmp");
}

BOOST_AUTO_TEST_CASE(append_data)
{
    check_append_data("data/coffi_test.obj", ".text$mn");
    check_append_data("data/demo.o", ".text");
    check_append_data("data/espui.dll", ".text");
    check_append_data("data/tclsh.exe", ".text");
    check_append_data("data/ti_c2000_1.obj", ".text");
}

//------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(build_file_pe)
{
    coffi c;
    c.create(COFFI_ARCHITECTURE_PE);
    BOOST_CHECK_EQUAL(c.save("data/build_file_pe.tmp"), true);
    check_binaries_equal("data/build_file_pe.expected", "data/build_file_pe.tmp");
}
