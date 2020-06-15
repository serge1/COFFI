#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include <boost/test/unit_test.hpp>

#include <coffi/coffi.hpp>
#include "coffio_test.hpp"

using namespace COFFI;
using namespace boost::unit_test;

//------------------------------------------------------------------------------

#define main main_writer
#define write_the_file write_the_file_writer
#include "examples/writer/writer.cpp"
#undef main
#undef write_the_file

BOOST_AUTO_TEST_CASE(example_writer)
{
    write_the_file_writer("data/answer_mingw32.exe.tmp");
    check_binaries_equal("data/answer_mingw32.exe.tmp", "data/answer_mingw32.exe.expected");
}

//------------------------------------------------------------------------------

#define main main_write_obj
#define write_the_file write_the_file_write_obj
#include "examples/write_obj/write_obj.cpp"
#undef main
#undef write_the_file

BOOST_AUTO_TEST_CASE(example_write_obj)
{
    write_the_file_write_obj("data/answer_mingw32.o.tmp");
    check_binaries_equal("data/answer_mingw32.o.tmp", "data/answer_mingw32.o.expected");
}

//------------------------------------------------------------------------------

#define main main_anonymizer
#include "examples/anonymizer/anonymizer.cpp"
#undef main

BOOST_AUTO_TEST_CASE(anonymizer)
{
    int argc = 3;
    const char *argv[] = {
        "anonymizer",
        "data/coffi_test.obj",
        "data/coffi_test.obj.anonymizer.tmp"
    };
    main_anonymizer(argc, const_cast<char**>(argv));
    check_dump("data/coffi_test.obj.anonymizer.tmp", "data/coffi_test.obj.anonymizer.dump.expected", false);
}

//------------------------------------------------------------------------------

#define main main_tutorial
#include "examples/tutorial/tutorial.cpp"
#undef main

BOOST_AUTO_TEST_CASE(tutorial)
{
    int argc = 2;
    const char *argv1[] = {
        "tutorial",
        "data/tclsh.exe",
    };
    main_tutorial(argc, const_cast<char**>(argv1));

    const char *argv2[] = {
        "tutorial",
        "data/ti_c2000_1.obj",
    };
    main_tutorial(argc, const_cast<char**>(argv2));
}
