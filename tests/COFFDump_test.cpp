#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#define BOOST_TEST_MAIN
#ifndef _MSC_VER
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE COFFIO_test

#include <cstdlib>
#include <boost/test/unit_test.hpp>

#include <coffi/coffi.hpp>

using namespace COFFI;
using namespace boost::unit_test;

//------------------------------------------------------------------------------

//! Return the dump path for a COFF file
static std::string dump_test_file_name( const char *file_to_dump )
{
    return std::string(file_to_dump) + ".dump.log";
}

//! Return the path for the expected result of the dump for a COFF file
static std::string expexted_dump_file_name( const char *file_to_dump )
{
    return std::string(file_to_dump) + ".dump.expected";
}

//! Call COFFDump, and return the exit code
static int RunCOFFDump( const char *file_to_dump )
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    std::string COFFDump_path = "..\\example\\COFFDump\\COFFDump.exe";
#else
    std::string COFFDump_path = "../example/COFFDump/COFFDump";
#endif
    std::string cmd = COFFDump_path + " " + file_to_dump + " > " + dump_test_file_name(file_to_dump);
    printf("Running: %s\n", cmd.c_str());
    int retval = std::system(cmd.c_str());
    return retval;
}

static void TestCOFFDump( const char *file_to_dump )
{
    // Call COFFDump, and check the exit code
    int retval = RunCOFFDump(file_to_dump);
    BOOST_CHECK_EQUAL(retval, 0);

    // Compare the dump with the expected dump result

    // Read the dump output
    std::ifstream result_file(dump_test_file_name(file_to_dump));
    std::string result((std::istreambuf_iterator<char>(result_file)), std::istreambuf_iterator<char>());

    // Read the expected dump output
    std::ifstream expected_file(expexted_dump_file_name(file_to_dump));
    std::string expected((std::istreambuf_iterator<char>(expected_file)), std::istreambuf_iterator<char>());

    // Check
    BOOST_CHECK_EQUAL(expected, result);
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( COFFDump_pe )
{
    TestCOFFDump( "data/notepad.exe" );
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( COFFDump_ceva )
{
    TestCOFFDump( "data/demo.o" );
}

//------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE( COFFDump_ti )
{
    TestCOFFDump( "data/ti_c2000_1.out" );
}
