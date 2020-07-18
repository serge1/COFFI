# Getting Started with COFFI

@page get_started Getting Started with COFFI

@tableofcontents

This guide assumes that you are a little bit familiar with the COFF binary file format.
See @ref extdoc for more details on the COFF binary file format.

@section reader COFF File Reader

The COFFI library is just normal C++ header files.
In order to use all its classes and types, simply include the main header file "coffi.hpp".
All COFFI library declarations reside in a namespace called "COFFI". This can be seen in the following example (the full source file is in @ref examples_tutorial):

- Include the coffi.hpp header file
- The #COFFI namespace usage

@snippetlineno tutorial/tutorial.cpp ex_reader_include

This section of the tutorial will explain how to work with the reader portion of the COFFI library.
The first step would be creating a coffi class instance. The coffi constructor has no parameters. The creation is normally followed by invoking the 'load' member method, passing it an COFF file name as a parameter:

- Create the @ref COFFI::coffi "coffi" class instance
- Initialize the instance by loading a COFF file.
    The function @ref COFFI::coffi.load() "load()" returns `true` if the COFF file was found and processed successfully. It returns `false` otherwise

@snippetlineno tutorial/tutorial.cpp ex_reader_create

All the COFF file header properties such as the architecture are accessible now.
To get the architecture of the file use:

- The member function @ref COFFI::coffi.get_architecture() "get_architecture()" returns the COFF architecture. Possible return values are listed in @ref COFFI::coffi_architecture_t "coffi_architecture_t.
- The member function @ref COFFI::coffi.get_optional_header() "get_optional_header()" returns the COFF file optional header.
    The member function @ref COFFI::optional_header() "get_magic()" returns the @ref COFFI::coff_optional_header_pe.magic "magic" field of the optional header.

@snippetlineno tutorial/tutorial.cpp ex_reader_archi

@note
The COFF types, flags and constants are defined in the coffi_types.hpp header file.
This file is included automatically into the project.
For example: #OH_MAGIC_PE32, #OH_MAGIC_PE32PLUS constants define values for PE32/PE32+ formats.

COFF binary files might consist of (some items are optional):

- File headers
- Data directories (for the for PE architecture only), including:
  - Import/export information
  - Resource information,
  - Etc.
- Sections, including:
  - A header
  - Some raw data
  - Relocations entries
  - Line number entries
- A symbol table
- A strings table

Each section has its own responsibility: some contains executable code, others program's data, and so on. See COFF binary format documentation for your specific architecture for purpose and content description of sections and segments.
The following code demonstrates how to find out the amount of sections the COFF file contains. The code also presents how to access particular section properties like names and sizes:

- The @ref COFFI::coffi.get_sections() "get_sections()" member of COFFI's `reader` object allows to retrieve the number of sections inside a given COFF file.
  It could also be used to get access to individual sections by using the subscript operator[], which returns a pointer to the corresponding section's interface.
- Use the C++ range-based `for` loop to loop through the sections given by @ref COFFI::coffi.get_sections() "get_sections()".
  Sections can also be addressed with the subscript operator[], by its @ref COFFI::sections.operator[](size_t) "number" or @ref COFFI::sections.operator[](const std::string&) "symbolic name".
- get_index(), get_name() and get_data_size() are member functions of the @ref COFFI::section "section" class.

@snippetlineno tutorial/tutorial.cpp ex_reader_sections

@section accessors COFF Data Accessors

To simplify creation and interpretation of specific COFF elements, the COFFI library provides accessor classes.

Currently, the following classes are available:

<table><tr>
    <th>COFF item accessed</th>
    <th>Class</th>
    <th>Basic structure
    <br>(maps directly to the the file)</th>
    <th>Applicable for
    <br>architecture</th>
</tr><tr>
    <td>MS-DOS header</td>
    <td>@ref COFFI::dos_header "dos_header"</td>
    <td>@ref COFFI::msdos_header "msdos_header"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_PE "PE"</td>
</tr><tr>
    <td rowspan=2>COFF file header</td>
    <td>@ref COFFI::coff_header_impl "coff_header_impl"</td>
    <td>@ref COFFI::coff_file_header "coff_file_header"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_PE "PE",
        @ref COFFI::COFFI_ARCHITECTURE_CEVA "CEVA"</td>
</tr><tr>
    <td>@ref COFFI::coff_header_impl_ti "coff_header_impl_ti"</td>
    <td>@ref COFFI::coff_file_header_ti "coff_file_header_ti"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_TI "TI"</td>
</tr><tr>
    <td rowspan=3>COFF optional file header</td>
    <td>@ref COFFI::optional_header_impl_pe "optional_header_impl_pe"</td>
    <td>@ref COFFI::coff_optional_header_pe "coff_optional_header_pe"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_PE "PE",
        @ref COFFI::COFFI_ARCHITECTURE_CEVA "CEVA"</td>
</tr><tr>
    <td>@ref COFFI::optional_header_impl_pe_plus "optional_header_impl_pe_plus"</td>
    <td>@ref COFFI::coff_optional_header_pe_plus "coff_optional_header_pe_plus"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_PE "PE"</td>
</tr><tr>
    <td>@ref COFFI::optional_header_impl_ti "optional_header_impl_ti"</td>
    <td>@ref COFFI::common_optional_header_ti "common_optional_header_ti"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_TI "TI"</td>
</tr><tr>
    <td rowspan=2>Windows NT file header</td>
    <td>@ref COFFI::win_header_impl "win_header_impl<win_header_pe>"</td>
    <td>@ref COFFI::win_header_pe "win_header_pe"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_PE "PE"</td>
</tr><tr>
    <td>@ref COFFI::win_header_impl "win_header_impl<win_header_pe_plus>"</td>
    <td>@ref COFFI::win_header_pe_plus "win_header_pe_plus"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_PE "PE"</td>
</tr><tr>
    <td rowspan=2>Section header</td>
    <td>@ref COFFI::section_impl "section_impl"</td>
    <td>@ref COFFI::section_header "section_header"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_PE "PE",
        @ref COFFI::COFFI_ARCHITECTURE_CEVA "CEVA"</td>
</tr><tr>
    <td>@ref COFFI::section_impl_ti "section_impl_ti"</td>
    <td>@ref COFFI::section_header_ti "section_header_ti"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_TI "TI"</td>
</tr><tr>
    <td>Data directory</td>
    <td>@ref COFFI::directory "directory"</td>
    <td>@ref COFFI::image_data_directory "image_data_directory"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_PE "PE"</td>
</tr><tr>
    <td rowspan=3>Relocation entry</td>
    <td rowspan=3>@ref COFFI::relocation "relocation"</td>
    <td>@ref COFFI::rel_entry "rel_entry"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_PE "PE"</td>
</tr><tr>
    <td>@ref COFFI::rel_entry_ceva "rel_entry_ceva"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_CEVA "CEVA"</td>
</tr><tr>
    <td>@ref COFFI::rel_entry_ti "rel_entry_ti"</td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_TI "TI"</td>
</tr><tr>
    <td rowspan=2>Symbol</td>
    <td rowspan=2>@ref COFFI::symbol "symbol"</td>
    <td>@ref COFFI::symbol_record "symbol_record"
    <br>See also:
      - @ref COFFI::auxiliary_symbol_record "auxiliary_symbol_record"
    </td>
    <td>All</td>
</tr><tr>
    <td>
      - @ref COFFI::auxiliary_symbol_record_1 "auxiliary_symbol_record_1"
      - @ref COFFI::auxiliary_symbol_record_2 "auxiliary_symbol_record_2"
      - @ref COFFI::auxiliary_symbol_record_3 "auxiliary_symbol_record_3"
      - @ref COFFI::auxiliary_symbol_record_4 "auxiliary_symbol_record_4"
      - @ref COFFI::auxiliary_symbol_record_5 "auxiliary_symbol_record_5"
    </td>
    <td>@ref COFFI::COFFI_ARCHITECTURE_PE "PE"</td>
</tr></table>

See also the @ref group_access "macros for accessing the COFF structures fields".

@section coffdump COFFDump Utility

The source code for the COFFDump Utility can be found in the `examples` directory.
An example of output is presented @ref hello_dump "below".

@section writer COFF File Writer

@subsection writer_source Writing the program

In this section we will create a simple COFF executable file that prints out the classical "Hello, World!" message.

The executable will be created and run on a Windows platform.
It is supposed to run well on both 32 and 64-bit Windows platforms.
The full source file is in @ref examples_writer.

The file will be created **without** invoking the compiler or assembler tools in the usual way
(i.e. translating high level source code that makes use of the standard library functions).

Instead, using the COFFI writer, all the necessary sections and data of the file will be created and filled explicitly, each, with its appropriate data.
The physical file would then be created by the COFFI library.

Before starting, implementations details of @ref COFFI::coffi "coffi" that users should be aware of are:

- The @ref COFFI::coffi "coffi" class, while constructing, creates the string table section automatically.
- The @ref COFFI::coffi "coffi" class computes and updates the offsets, sizes, etc., either while constructing, or when saving the file.
    The @ref COFFI::coffi "coffi" class tries to keep the various sections and other elements in the same order as given by the user.
  
Our usage of the library API will consist of several steps:

- Creating an empty coffi object
- Setting-up COFF file properties
- Creating code section and data content for it
- Creating data section and its content
- Addition of both sections to corresponding COFF file segments
- Setting-up the program's entry point
- Dumping the coffi object to an executable COFF file

Initialize an empty @ref COFFI::coffi "coffi" object.
This should be done as the first step when creating a new @ref COFFI::coffi "coffi" object, because the other API is relying on parameter provided (COFF file architecture).

@snippetlineno writer/writer.cpp ex_writer_open

Initialize an optional header.

@snippetlineno writer/writer.cpp ex_writer_optional

Create a new section, set section's attributes. Section type, flags and alignment have a big significance and controls how this section is treated by a linker or OS loader.
This code section contains the code that:

- Sets the `MessageBoxA` parameters
- Calls `MessageBoxA`
- Sets the `ExitProcess` parameter
- Calls `ExitProcess`

The `ExitProcess` and `MessageBoxA` functions are imported from the Windows DLL at adresses `0x40304C` and  `0x403054`, see IAT  (import address table) below.

@snippetlineno writer/writer.cpp ex_writer_code

Add a data section, storing the overwhelmingly impressive message.

@snippetlineno writer/writer.cpp ex_writer_rdata


Add a data section, used by Windows for importing DLL (run-time link).
Refer to the Windows documentation for more details.
The code below imports `ExitProcess` and `MessageBoxA` from `KERNEL32.dll` and `USER32.dll` respectively.

@snippetlineno writer/writer.cpp ex_writer_idata

Add a relocation section, used by Windows for loading the file.
The relocation entries identify the places in the code section where there are addresses to be relocated.

@snippetlineno writer/writer.cpp ex_writer_reloc

Update the file headers

@snippetlineno writer/writer.cpp ex_writer_header

Update the data directories, especially those pointing to the DLL import information and relocation information.

@snippetlineno writer/writer.cpp ex_writer_directories

Save the COFF binary file on disk:

@snippetlineno writer/writer.cpp ex_writer_save

@note
The coffi library takes care of the resulting binary file layout calculation.
It does this on base of the provided memory image addresses and sizes.
It is the user's responsibility to provide correct values for these parameters.
Please refer to the documentation of the COFF architectures for specific requirements related to the executable COFF file attributes and/or mapping.

Similarly to the @ref reader example, you may use provided accessor classes to interpret and modify content of section's data.

@subsection writer_test Testing the program

Let's compile the example above (see the complete source file @ref examples_writer) into an executable file (named `writer.exe`).
Invoking `writer.exe` will create the executable file `hello.exe` that prints the overwhelmingly impressive "Hello, World!" message.

The listing below works with GCC installed:

```console
C:\the_user_path\COFFI\examples\writer>dir /B *.cpp
writer.cpp

C:\the_user_path\COFFI\examples\writer>g++ writer.cpp -o writer.exe -I../..

C:\the_user_path\COFFI\examples\writer>writer.exe

C:\the_user_path\COFFI\examples\writer>dir /B *.exe
hello.exe
writer.exe

C:\the_user_path\COFFI\examples\writer>hello.exe
```

@htmlonly
<center><img src="hello.png" title="The overwhelmingly impressive message"></center>
@endhtmlonly

The same thing with Visual C++:

```console
C:\the_user_path\COFFI\examples\writer>vcvars32.bat
**********************************************************************
** Visual Studio 2019 Developer Command Prompt v16.6.2
** Copyright (c) 2020 Microsoft Corporation
**********************************************************************
[vcvarsall.bat] Environment initialized for: 'x86'

C:\the_user_path\COFFI\examples\writer>cl writer.cpp /I../.. /EHsc /nologo
writer.cpp

C:\the_user_path\COFFI\examples\writer>writer.exe

C:\the_user_path\COFFI\examples\writer>hello.exe
```

In case you already compiled the @ref coffdump, you can inspect the properties of the produced executable file:

```console
C:\the_user_path\COFFI\examples\writer>..\COFFDump\COFFDump.exe hello.exe > hello.dump

C:\the_user_path\COFFI\examples\writer>type hello.dump
```

Will result in:

@anchor hello_dump
@include writer/hello.dump
