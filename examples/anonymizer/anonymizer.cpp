/*
anonymizer.cpp - Overwrites all data from a COFF file with random data.

Copyright (C) 2017 by Martin Bickel

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

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#define COFFI_NO_INTTYPES
#endif

#include <string>
#include <iostream>
#include <coffi/coffi.hpp>
#include <fstream>
#include <random>

using namespace COFFI;

bool preserve_name(const std::string& name)
{
    static std::vector<std::string> names_to_preserve = {
        ".shstrtab", ".rodata", ".bss", ".data", ".text", ".text_vle"};

    for (auto s = names_to_preserve.begin(); s != names_to_preserve.end(); ++s)
        if (*s == name)
            return true;
    return false;
}

std::default_random_engine         generator(0xe1f);
std::uniform_int_distribution<int> distribution(0, 255);

void randomize_data(section* s)
{
    uint32_t size = s->get_data_size();
    if (size > 0) {
        char new_data[size];
        for (uint32_t i = 0; i < size; ++i) {
            new_data[i] = distribution(generator);
        }
        s->set_data(new_data, size);
    }
}

std::string generate(int length)
{
    static uint32_t counter = 0;
    std::string     s       = std::to_string(counter);
    while (s.length() < length) {
        s = "s" + s;
    }
    s = s.substr(0, length);
    counter++;
    return s;
}

void processStringTable(coffi& reader)
{
    std::cout << "Info: processing string table section" << std::endl;
    uint32_t    size    = reader.get_strings_size();
    const char* pos_src = reader.get_strings() + 4;
    const char* pos_end = reader.get_strings() + size;
    char        new_strings[size];
    char*       pos_dst = new_strings + 4;
    int         counter = 0;
    while (pos_src < pos_end) {
        auto len = strlen(pos_src);
        if (preserve_name(pos_src)) {
            std::strcpy(pos_dst, pos_src);
        }
        else {
            std::strcpy(pos_dst, generate(len).c_str());
        }
        pos_src += len + 1;
        pos_dst += len + 1;
        counter++;
    }
    reader.set_strings(new_strings, size);
    std::cout << counter << " strings found" << std::endl;
}

int main(int argc, char** argv)
{
    try {
        if (argc != 3) {
            std::cout << "Usage: anonymizer <file_name> <output_file_name>\n";
            return 1;
        }

        std::string filename       = argv[1];
        std::string outputfilename = argv[2];

        coffi reader;

        if (!reader.load(filename)) {
            std::cerr << "File " << filename
                      << " is not found or it is not a COFF file\n";
            return 1;
        }

        for (auto s : reader.get_sections()) {
            if ((s->get_flags() & IMAGE_SCN_CNT_CODE) ||
                (s->get_flags() & IMAGE_SCN_CNT_INITIALIZED_DATA)) {
                randomize_data(s);
                std::cout << "Sanitized " << s->get_data_size()
                          << " bytes in section" << std::endl;
            }
        }

        if (reader.get_symbols()->size() > 0) {
            std::cout << "Skipping symbol table." << std::endl;
        }

        processStringTable(reader);

        if (!reader.save(outputfilename)) {
            std::cerr << "Cannot write in file " << outputfilename << "\n";
            return 1;
        }

        return 0;
    }
    catch (const std::string& s) {
        std::cerr << s << std::endl;
    }
    catch (const char* s) {
        std::cerr << s << std::endl;
    }
    return 1;
}
