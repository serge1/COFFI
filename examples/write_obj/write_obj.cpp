/*
 * This example shows how to create a PE object file for Windows
 *
 * Instructions:
 * 1. Compile and link this file with the COFFI library
 *    g++ write_obj.cpp -o writer_obj.exe
 * 2. Execute the result file write_obj.exe
 *    write_obj.exe
 *    It will produce answer.o
 * 3. Link the output file answer.o, with a test program
 *    gcc answer.o answer_test.c -o answer.exe
 * 4. Run the result file:
 *    answer.exe
 *    It should print "The answer is 42"
 */

#include <string>
#include <coffi/coffi.hpp>

using namespace COFFI;

void write_the_file(const std::string& filename)
{
    coffi writer;

    // You can't proceed before this function call!
    writer.create(COFFI_ARCHITECTURE_PE);

    writer.get_header()->set_flags(IMAGE_FILE_32BIT_MACHINE |
                                   IMAGE_FILE_LINE_NUMS_STRIPPED);

    // Create code section
    section* text_sec = writer.add_section(".text");
    // Code that returns 42
    char text[] = {
        '\x55',                                 // push   %ebp
        '\x89', '\xE5',                         // mov    %esp,%ebp
        '\x83', '\xE4', '\xF0',                 // and    $0xfffffff0,%esp
        '\xE8', '\x00', '\x00', '\x00', '\x00', // call   b <_main+0xb>
        '\xB8', '\x2A', '\x00', '\x00', '\x00', // mov    $0x2a,%eax
        '\xC9',                                 // leave
        '\xC3',                                 // ret
        '\x90',                                 // nop
        '\x90',                                 // nop
    };
    text_sec->set_data(text, sizeof(text));
    //text_sec->set_virtual_address(0x1000);
    //text_sec->set_virtual_size(sizeof(text));
    text_sec->set_flags(IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ |
                        IMAGE_SCN_CNT_CODE | IMAGE_SCN_ALIGN_4BYTES);

    // Create a .data section, empty
    section* data_sec = writer.add_section(".data");
    data_sec->set_flags(IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE |
                        IMAGE_SCN_CNT_INITIALIZED_DATA |
                        IMAGE_SCN_ALIGN_4BYTES);

    // Create a .bss section, empty
    section* bss_sec = writer.add_section(".bss");
    bss_sec->set_flags(IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE |
                       IMAGE_SCN_CNT_UNINITIALIZED_DATA |
                       IMAGE_SCN_ALIGN_4BYTES);

    // Create a section with arbitrary data
    section* v_sec = writer.add_section(".rdata$zzz");
    char     v[]   = "This is an arbitrary read-only string data\0";
    v_sec->set_data(v, sizeof(v));
    v_sec->set_flags(IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA |
                     IMAGE_SCN_ALIGN_4BYTES);

    symbol* sym1 = writer.add_symbol(".file");
    sym1->set_section_number(IMAGE_SYM_DEBUG);
    sym1->set_storage_class(IMAGE_SYM_CLASS_FILE);
    sym1->set_aux_symbols_number(1);
    auxiliary_symbol_record_4 a1{"answer.c"};
    sym1->get_auxiliary_symbols().push_back(*(auxiliary_symbol_record*)&a1);

    symbol* sym2 = writer.add_symbol("_answer");
    sym2->set_type(IMAGE_SYM_TYPE_FUNCTION);
    sym2->set_storage_class(IMAGE_SYM_CLASS_EXTERNAL);
    sym2->set_section_number(text_sec->get_index() + 1);
    sym2->set_aux_symbols_number(1);
    auxiliary_symbol_record a2{
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    sym2->get_auxiliary_symbols().push_back(a2);

    symbol* sym3 = writer.add_symbol(".text");
    sym3->set_type(IMAGE_SYM_TYPE_NOT_FUNCTION);
    sym3->set_storage_class(IMAGE_SYM_CLASS_STATIC);
    sym3->set_section_number(text_sec->get_index() + 1);
    sym3->set_aux_symbols_number(1);
    auxiliary_symbol_record_5 a3{
        text_sec->get_data_size(), 1, 0, 0, 0, 0, {0, 0, 0}};
    sym3->get_auxiliary_symbols().push_back(
        *reinterpret_cast<auxiliary_symbol_record*>(&a3));

    symbol* sym4 = writer.add_symbol(".data");
    sym4->set_type(IMAGE_SYM_TYPE_NOT_FUNCTION);
    sym4->set_storage_class(IMAGE_SYM_CLASS_STATIC);
    sym4->set_section_number(data_sec->get_index() + 1);
    sym4->set_aux_symbols_number(1);
    auxiliary_symbol_record_5 a4{
        data_sec->get_data_size(), 0, 0, 0, 0, 0, {0, 0, 0}};
    sym4->get_auxiliary_symbols().push_back(
        *reinterpret_cast<auxiliary_symbol_record*>(&a4));

    symbol* sym5 = writer.add_symbol(".bss");
    sym5->set_type(IMAGE_SYM_TYPE_NOT_FUNCTION);
    sym5->set_storage_class(IMAGE_SYM_CLASS_STATIC);
    sym5->set_section_number(bss_sec->get_index() + 1);
    sym5->set_aux_symbols_number(1);
    auxiliary_symbol_record_5 a5{
        bss_sec->get_data_size(), 0, 0, 0, 0, 0, {0, 0, 0}};
    sym5->get_auxiliary_symbols().push_back(
        *reinterpret_cast<auxiliary_symbol_record*>(&a5));

    symbol* sym6 = writer.add_symbol(".rdata$zzz");
    sym6->set_type(IMAGE_SYM_TYPE_NOT_FUNCTION);
    sym6->set_storage_class(IMAGE_SYM_CLASS_STATIC);
    sym6->set_section_number(v_sec->get_index() + 1);
    sym6->set_aux_symbols_number(1);
    auxiliary_symbol_record_5 a6{
        v_sec->get_data_size(), 0, 0, 0, 0, 0, {0, 0, 0}};
    sym6->get_auxiliary_symbols().push_back(
        *reinterpret_cast<auxiliary_symbol_record*>(&a6));

    // Create the object file
    writer.save(filename);
}

int main()
{
    write_the_file("answer.o");
    return 0;
}
