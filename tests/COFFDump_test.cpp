#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include <cstdlib>
#include <regex>
#include <boost/test/unit_test.hpp>

#include <coffi/coffi.hpp>
#include "coffio_test.hpp"

using namespace COFFI;
using namespace boost::unit_test;

//------------------------------------------------------------------------------

//! Return the dump path for a COFF file
std::string dump_test_file_name( const std::string &file_to_dump )
{
    return file_to_dump + ".dump.tmp";
}

//! Return the path for the expected result of the dump for a COFF file
std::string expected_dump_file_name( const std::string &file_to_dump )
{
    return file_to_dump + ".dump.expected";
}

//! Call COFFDump, and return the exit code
void RunCOFFDump( const std::string &file_to_dump )
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    std::string COFFDump_path = "..\\examples\\COFFDump\\COFFDump.exe";
#else
    std::string COFFDump_path = "../examples/COFFDump/COFFDump";
#endif
    std::string cmd = COFFDump_path + " " + file_to_dump + " > " + dump_test_file_name(file_to_dump);
    printf("Running: %s\n", cmd.c_str());
    int retval = std::system(cmd.c_str());
    BOOST_CHECK_EQUAL(retval, 0);
}

void check_dumps_equal(const std::string &dump1, const std::string &dump2, bool compare_checksum)
{
    // Read the expected dump output
    std::ifstream dump1_file(dump1);
    std::string str1((std::istreambuf_iterator<char>(dump1_file)), std::istreambuf_iterator<char>());

    // Read the file dump output
    std::ifstream dump2_file(dump2);
    std::string str2((std::istreambuf_iterator<char>(dump2_file)), std::istreambuf_iterator<char>());

    // Replace the end-of-lines
    str1 = std::regex_replace(str1, std::regex("(?:\\r\\n|\\r)"), "\\n");
    str2 = std::regex_replace(str2, std::regex("(?:\\r\\n|\\r)"), "\\n");

    // Replace the file name
    str1 = std::regex_replace(str1, std::regex("Dump of file .*"), "Dump of file ****");
    str2 = std::regex_replace(str2, std::regex("Dump of file .*"), "Dump of file ****");

    // Replace the timestamps
    str1 = std::regex_replace(str1, std::regex("(TimeDateStamp: *).*"), "\\1 ****");
    str2 = std::regex_replace(str2, std::regex("(TimeDateStamp: *).*"), "\\1 ****");

    if (!compare_checksum) {
        str1 = std::regex_replace(str1, std::regex("(\\n *checksum *)[0-9A-Fa-f]+(\\s*\\n)"), "\\1****\\2");
        str2 = std::regex_replace(str2, std::regex("(\\n *checksum *)[0-9A-Fa-f]+(\\s*\\n)"), "\\1****\\2");
    }

    // Check Dumps
    printf("Comparing %s and %s\n", dump1.c_str(), dump2.c_str());
    BOOST_CHECK_EQUAL(str1 == str2, true);
}

void check_dump(const std::string &file, const std::string &expected_dump, bool compare_checksum)
{
    coffi c;

    // Run COFFDump
    RunCOFFDump(file);

    check_dumps_equal(expected_dump, dump_test_file_name(file), compare_checksum);
}

static void TestCOFFDump( const std::string &file_to_dump )
{
    check_dump(file_to_dump, expected_dump_file_name(file_to_dump));
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( COFFDump_pe )
{
    TestCOFFDump("data/anobj.o");
    TestCOFFDump("data/coffi_test.obj");
    TestCOFFDump("data/espui.dll");
    TestCOFFDump("data/label.exe");
    TestCOFFDump("data/NikPEViewer.exe");
    TestCOFFDump("data/notepad.exe");
    TestCOFFDump("data/tclsh.exe");
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( COFFDump_ceva )
{
    TestCOFFDump("data/demo.o");
    TestCOFFDump("data/demo.a");
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( COFFDump_ti )
{
    TestCOFFDump("data/ti_c2000_1.obj");
    TestCOFFDump("data/ti_c2000_1.out");
}
