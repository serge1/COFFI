#ifndef COFFIO_TEST_H
#define COFFIO_TEST_H

#include <string>

//! Return the dump path for a COFF file
extern std::string dump_test_file_name(const std::string &file_to_dump);

//! Return the path for the expected result of the dump for a COFF file
extern std::string expected_dump_file_name(const std::string &file_to_dump);

//! Call COFFDump, and check the exit code
extern void RunCOFFDump(const std::string &file_to_dump);

//! Check if dumps are equal
extern void check_dumps_equal(const std::string &dump1, const std::string &dump2, bool compare_checksum = true);

//! Call COFFDump, and check the result file
extern void check_dump(const std::string &file, const std::string &expected_dump, bool compare_checksum = true);

//! Test equality of 2 binary files
extern void check_binaries_equal(const std::string &file1, const std::string &file2);

//! Temporarily suppresse the standard output
void supress_stdout();

//! Restore the standard output
void resume_stdout();

#endif //COFFIO_TEST_H
