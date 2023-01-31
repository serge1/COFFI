/*
Copyright (C) 2014-2014 by Serge Lamikhov-Center

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

/*! @file coffi_headers.hpp 
 * @brief COFFI library classes for the COFF file headers.
 *
 * Do not include this file directly. This file is included by coffi.hpp.
 */

#ifndef COFFI_WIN_HEADER_HPP
#define COFFI_WIN_HEADER_HPP

#include <string>
#include <iostream>

#include <coffi/coffi_utils.hpp>

#if defined(__has_include) && __has_include(<gsl/narrow>)
#include <gsl/narrow>
using gsl::narrow_cast;
#else
#ifndef narrow_cast
#define narrow_cast static_cast
#endif
#endif

namespace COFFI {

//------------------------------------------------------------------------------
//! Class for accessing the MS-DOS file header
class dos_header
{
  public:
    //------------------------------------------------------------------------------
    dos_header()
    {
        std::fill_n(reinterpret_cast<char*>(&header), sizeof(header), '\0');
        static const uint8_t default_stub[] = {
            0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD, 0x21, 0xB8, 0x01,
            0x4C, 0xCD, 0x21, 0x54, 0x68, 0x69, 0x73, 0x20, 0x70, 0x72, 0x6F,
            0x67, 0x72, 0x61, 0x6D, 0x20, 0x63, 0x61, 0x6E, 0x6E, 0x6F, 0x74,
            0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6E, 0x20, 0x69, 0x6E, 0x20,
            0x44, 0x4F, 0x53, 0x20, 0x6D, 0x6F, 0x64, 0x65, 0x2E, 0x0D, 0x0D,
            0x0A, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        stub_      = 0;
        stub_size_ = 0;
        std::fill_n(reinterpret_cast<char*>(&header), sizeof(header), '\0');
        set_stub(reinterpret_cast<const char*>(default_stub),
                 sizeof(default_stub));
        set_signature('M' + ('Z' << 8));
        set_bytes_in_last_block(144);
        set_blocks_in_file(3);
        set_header_paragraphs(4);
        set_max_extra_paragraphs(65535);
        set_sp(184);
        set_reloc_table_offset(64);
        set_pe_sign_location(sizeof(msdos_header) + get_stub_size());
    }
    virtual ~dos_header() { clean(); }

    //------------------------------------------------------------------------------
    //! @accessors{dos_header}
    COFFI_GET_SET_ACCESS(uint16_t, signature);
    COFFI_GET_SET_ACCESS(uint16_t, bytes_in_last_block);
    COFFI_GET_SET_ACCESS(uint16_t, blocks_in_file);
    COFFI_GET_SET_ACCESS(uint16_t, num_relocs);
    COFFI_GET_SET_ACCESS(uint16_t, header_paragraphs);
    COFFI_GET_SET_ACCESS(uint16_t, min_extra_paragraphs);
    COFFI_GET_SET_ACCESS(uint16_t, max_extra_paragraphs);
    COFFI_GET_SET_ACCESS(uint16_t, ss);
    COFFI_GET_SET_ACCESS(uint16_t, sp);
    COFFI_GET_SET_ACCESS(uint16_t, checksum);
    COFFI_GET_SET_ACCESS(uint16_t, ip);
    COFFI_GET_SET_ACCESS(uint16_t, cs);
    COFFI_GET_SET_ACCESS(uint16_t, reloc_table_offset);
    COFFI_GET_SET_ACCESS(uint16_t, overlay_number);
    COFFI_GET_SET_ACCESS(uint16_t, oem_id);
    COFFI_GET_SET_ACCESS(uint16_t, oem_info);
    COFFI_GET_SET_ACCESS(int32_t, pe_sign_location);

    COFFI_GET_SIZEOF();
    //! @endaccessors

    //------------------------------------------------------------------------------
    bool load(std::istream& stream)
    {
        clean();

        // Read EXE file signature
        char e_ident0[CI_NIDENT0];
        stream.read(reinterpret_cast<char*>(&e_ident0), sizeof(e_ident0));
        if (stream.gcount() != sizeof(e_ident0)) {
            return false;
        }

        // Is it EXE file?
        if (e_ident0[CI_MAG0] != PEMAG0 || e_ident0[CI_MAG1] != PEMAG1) {
            return false;
        }

        stream.seekg(0);
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (stream.gcount() != sizeof(header)) {
            return false;
        }

        if (get_pe_sign_location() > static_cast<int32_t>(sizeof(header))) {
            stub_size_      = get_pe_sign_location() - sizeof(header);
            char* read_stub = new char[stub_size_];
            if (!read_stub) {
                return false;
            }
            stream.read(read_stub, stub_size_);
            stub_ = read_stub;
            if (stream.gcount() != stub_size_) {
                return false;
            }
        }
        else {
            stream.seekg(get_pe_sign_location());
        }

        char e_ident1[CI_NIDENT1];
        stream.read(e_ident1, CI_NIDENT1);
        if (stream.gcount() != CI_NIDENT1 || e_ident1[CI_MAG2] != PEMAG2 ||
            e_ident1[CI_MAG3] != PEMAG3 || e_ident1[CI_MAG4] != PEMAG4 ||
            e_ident1[CI_MAG5] != PEMAG5) {
            return false;
        }

        return true;
    }

    //------------------------------------------------------------------------------
    void save(std::ostream& stream)
    {
        stream.write(reinterpret_cast<char*>(&header), sizeof(header));
        stream.write(stub_, stub_size_);
        stream.put(PEMAG2);
        stream.put(PEMAG3);
        stream.put(PEMAG4);
        stream.put(PEMAG5);
    }

    //------------------------------------------------------------------------------
    virtual const char* get_stub() const { return stub_; }

    //------------------------------------------------------------------------------
    virtual uint32_t get_stub_size() const { return stub_size_; }

    //------------------------------------------------------------------------------
    void set_stub(const char* data, uint32_t size)
    {
        if (stub_) {
            delete[] stub_;
        }
        stub_size_     = size;
        char* new_stub = new char[stub_size_];
        if (!new_stub) {
            stub_size_ = 0;
        }
        else {
            std::copy(data, data + size, new_stub);
        }
        stub_ = new_stub;
    }

    //------------------------------------------------------------------------------
    void set_stub(const std::string& data)
    {
        set_stub(data.c_str(), narrow_cast<uint32_t>(data.size()));
    }

    //------------------------------------------------------------------------------
  private:
    //------------------------------------------------------------------------------
    void clean()
    {
        if (stub_) {
            delete[] stub_;
            stub_ = 0;
        }
        stub_size_ = 0;
    }

    //------------------------------------------------------------------------------
    msdos_header header;
    const char*  stub_;
    int          stub_size_;
};

//------------------------------------------------------------------------------
//! Interface class for accessing the COFF file header, for all the COFF architectures.
class coff_header
{
  public:
    virtual ~coff_header() {}

    //! @accessors{coff_header}
    COFFI_GET_SET_ACCESS_DECL(uint16_t, version);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, machine);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, sections_count);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, time_data_stamp);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, symbol_table_offset);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, symbols_count);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, optional_header_size);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, flags);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, target_id);

    COFFI_GET_SIZEOF_DECL();
    //! @endaccessors

    virtual bool load(std::istream& stream) = 0;
    virtual void save(std::ostream& stream) = 0;
};

//------------------------------------------------------------------------------
/*! @brief Template class for accessing a COFF file header, depends on the underlying COFF file header format.
     *
     * The template parameter **class T** is one of:
     *   - coff_file_header:    @copybrief coff_file_header   
     *   - coff_file_header_ti: @copybrief coff_file_header_ti
     */
template <class T> class coff_header_impl_tmpl : public coff_header
{
  public:
    coff_header_impl_tmpl()
    {
        std::fill_n(reinterpret_cast<char*>(&header), sizeof(header), '\0');
    }

    //------------------------------------------------------------------------------
    //! @accessors{coff_header_impl_tmpl}
    COFFI_GET_SET_ACCESS(uint16_t, sections_count);
    COFFI_GET_SET_ACCESS(uint32_t, time_data_stamp);
    COFFI_GET_SET_ACCESS(uint32_t, symbol_table_offset);
    COFFI_GET_SET_ACCESS(uint32_t, symbols_count);
    COFFI_GET_SET_ACCESS(uint16_t, optional_header_size);
    COFFI_GET_SET_ACCESS(uint16_t, flags);

    COFFI_GET_SIZEOF();
    //! @endaccessors

    //------------------------------------------------------------------------------
    bool load(std::istream& stream)
    {
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (stream.gcount() != sizeof(header)) {
            return false;
        }

        return true;
    }

    //------------------------------------------------------------------------------
    void save(std::ostream& stream)
    {
        stream.write(reinterpret_cast<char*>(&header), sizeof(header));
    }

    //------------------------------------------------------------------------------
  protected:
    T header;
};

//------------------------------------------------------------------------------
//! Class for accessing a COFF file header, for the PE format.
class coff_header_impl : public coff_header_impl_tmpl<coff_file_header>
{
  public:
    //! @accessors{coff_header_impl}
    COFFI_GET_SET_ACCESS(uint16_t, machine);
    COFFI_GET_SET_ACCESS_NONE(uint16_t, version);
    COFFI_GET_SET_ACCESS_NONE(uint16_t, target_id);
    //! @endaccessors
};

//------------------------------------------------------------------------------
//! Class for accessing a COFF file header, for the Texas Instruments format.
class coff_header_impl_ti : public coff_header_impl_tmpl<coff_file_header_ti>
{
  public:
    //! @accessors{coff_header_impl_ti}
    COFFI_GET_SET_ACCESS(uint16_t, version);
    COFFI_GET_SET_ACCESS(uint16_t, target_id);
    COFFI_GET_SET_ACCESS_NONE(uint16_t, machine);
    //! @endaccessors
};

//------------------------------------------------------------------------------
//! Interface class for accessing the COFF file optional header, for all the COFF architectures.
class optional_header
{
  public:
    virtual ~optional_header(){};

    //! @accessors{optional_header}
    COFFI_GET_SET_ACCESS_DECL(uint16_t, magic);
    COFFI_GET_SET_ACCESS_DECL(uint8_t, major_linker_version);
    COFFI_GET_SET_ACCESS_DECL(uint8_t, minor_linker_version);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, linker_version);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, code_size);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, initialized_data_size);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, uninitialized_data_size);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, entry_point_address);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, code_base);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, data_base);

    COFFI_GET_SIZEOF_DECL();
    //! @endaccessors

    virtual bool load(std::istream& stream) = 0;
    virtual void save(std::ostream& stream) = 0;
};

//------------------------------------------------------------------------------
/*! @brief Template class for accessing a COFF file optional header, depends on the underlying COFF file optional header format.
     *
     * The template parameter **class T** is one of:
     *   - coff_optional_header_pe:      @copybrief coff_optional_header_pe
     *   - coff_optional_header_pe_plus: @copybrief coff_optional_header_pe_plus
     *   - common_optional_header_ti:    @copybrief common_optional_header_ti
     */
template <class T> class optional_header_impl_tmpl : public optional_header
{
  public:
    optional_header_impl_tmpl()
    {
        std::fill_n(reinterpret_cast<char*>(&header), sizeof(header), '\0');
    }

    //------------------------------------------------------------------------------

    //! @accessors{optional_header_impl_tmpl}
    COFFI_GET_SET_ACCESS(uint16_t, magic);
    COFFI_GET_SET_ACCESS(uint32_t, code_size);
    COFFI_GET_SET_ACCESS(uint32_t, initialized_data_size);
    COFFI_GET_SET_ACCESS(uint32_t, uninitialized_data_size);
    COFFI_GET_SET_ACCESS(uint32_t, entry_point_address);
    COFFI_GET_SET_ACCESS(uint32_t, code_base);

    COFFI_GET_SIZEOF();
    //! @endaccessors

    //---------------------------------------------------------------------
    bool load(std::istream& stream)
    {
        std::fill_n(reinterpret_cast<char*>(&header), sizeof(header), '\0');
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (stream.gcount() != sizeof(header)) {
            return false;
        }
        return true;
    }

    //------------------------------------------------------------------------------
    void save(std::ostream& stream)
    {
        stream.write(reinterpret_cast<char*>(&header), sizeof(header));
    }

    //------------------------------------------------------------------------------
    T header;
};

//------------------------------------------------------------------------------
//! Class for accessing a COFF file optional header, for the PE32 format.
class optional_header_impl_pe
    : public optional_header_impl_tmpl<coff_optional_header_pe>
{
  public:
    //! @accessors{optional_header_impl_pe}
    COFFI_GET_SET_ACCESS(uint8_t, major_linker_version);
    COFFI_GET_SET_ACCESS(uint8_t, minor_linker_version);
    COFFI_GET_SET_ACCESS_NONE(uint16_t, linker_version);
    COFFI_GET_SET_ACCESS(uint32_t, data_base);
    //! @endaccessors
};

//------------------------------------------------------------------------------
//! Class for accessing a COFF file optional header, for the PE32+ format.
class optional_header_impl_pe_plus
    : public optional_header_impl_tmpl<coff_optional_header_pe_plus>
{
  public:
    //! @accessors{optional_header_impl_pe_plus}
    COFFI_GET_SET_ACCESS(uint8_t, major_linker_version);
    COFFI_GET_SET_ACCESS(uint8_t, minor_linker_version);
    COFFI_GET_SET_ACCESS_NONE(uint16_t, linker_version);
    COFFI_GET_SET_ACCESS_NONE(uint32_t, data_base);
    //! @endaccessors
};

//------------------------------------------------------------------------------
//! Class for accessing a COFF file optional header, for the Texas Instruments format.
class optional_header_impl_ti
    : public optional_header_impl_tmpl<common_optional_header_ti>
{
  public:
    //! @accessors{optional_header_impl_ti}
    COFFI_GET_SET_ACCESS(uint16_t, linker_version);
    COFFI_GET_SET_ACCESS_NONE(uint8_t, major_linker_version);
    COFFI_GET_SET_ACCESS_NONE(uint8_t, minor_linker_version);
    COFFI_GET_SET_ACCESS(uint32_t, data_base);
    //! @endaccessors
};

//------------------------------------------------------------------------------
//! Interface class for accessing the Windows NT file header, for both the PE32 and PE32+ formats.
class win_header
{
  public:
    virtual ~win_header(){};

    //! @accessors{win_header}
    COFFI_GET_SET_ACCESS_DECL(uint64_t, image_base);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, section_alignment);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, file_alignment);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, major_os_version);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, minor_os_version);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, major_image_version);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, minor_image_version);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, major_subsystem_version);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, minor_subsystem_version);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, win32_version_value);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, image_size);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, headers_size);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, checksum);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, subsystem);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, dll_flags);
    COFFI_GET_SET_ACCESS_DECL(uint64_t, stack_reserve_size);
    COFFI_GET_SET_ACCESS_DECL(uint64_t, stack_commit_size);
    COFFI_GET_SET_ACCESS_DECL(uint64_t, heap_reserve_size);
    COFFI_GET_SET_ACCESS_DECL(uint64_t, heap_commit_size);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, loader_flags);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, number_of_rva_and_sizes);

    COFFI_GET_SIZEOF_DECL();
    //! @endaccessors

    virtual bool load(std::istream& f) = 0;
    virtual void save(std::ostream& f) = 0;
};

//------------------------------------------------------------------------------
/*! @brief Template class for accessing a Windows NT file header, depends on the format (PE32 or PE32+).
     *
     * The template parameter **class T** is one of:
     *   - win_header_pe: For the PE32 format
     *   - win_header_pe_plus: For the PE32+ format
     */
template <class T> class win_header_impl : public win_header
{
  public:
    //------------------------------------------------------------------------------
    win_header_impl()
    {
        std::fill_n(reinterpret_cast<char*>(&header), sizeof(header), '\0');
        set_image_base(0x00400000);
        set_file_alignment(512);
    }

    //------------------------------------------------------------------------------
    //! @accessors{win_header_impl}
    COFFI_GET_SET_ACCESS(uint64_t, image_base);
    COFFI_GET_SET_ACCESS(uint32_t, section_alignment);
    COFFI_GET_SET_ACCESS(uint32_t, file_alignment);
    COFFI_GET_SET_ACCESS(uint16_t, major_os_version);
    COFFI_GET_SET_ACCESS(uint16_t, minor_os_version);
    COFFI_GET_SET_ACCESS(uint16_t, major_image_version);
    COFFI_GET_SET_ACCESS(uint16_t, minor_image_version);
    COFFI_GET_SET_ACCESS(uint16_t, major_subsystem_version);
    COFFI_GET_SET_ACCESS(uint16_t, minor_subsystem_version);
    COFFI_GET_SET_ACCESS(uint32_t, win32_version_value);
    COFFI_GET_SET_ACCESS(uint32_t, image_size);
    COFFI_GET_SET_ACCESS(uint32_t, headers_size);
    COFFI_GET_SET_ACCESS(uint32_t, checksum);
    COFFI_GET_SET_ACCESS(uint16_t, subsystem);
    COFFI_GET_SET_ACCESS(uint16_t, dll_flags);
    COFFI_GET_SET_ACCESS(uint64_t, stack_reserve_size);
    COFFI_GET_SET_ACCESS(uint64_t, stack_commit_size);
    COFFI_GET_SET_ACCESS(uint64_t, heap_reserve_size);
    COFFI_GET_SET_ACCESS(uint64_t, heap_commit_size);
    COFFI_GET_SET_ACCESS(uint32_t, loader_flags);
    COFFI_GET_SET_ACCESS(uint32_t, number_of_rva_and_sizes);

    COFFI_GET_SIZEOF();
    //! @endaccessors

    //------------------------------------------------------------------------------
    bool load(std::istream& stream) override
    {
        int read_size = sizeof(header);
        std::fill_n(reinterpret_cast<char*>(&header), read_size, '\0');
        stream.read(reinterpret_cast<char*>(&header), read_size);
        if (stream.gcount() != read_size) {
            return false;
        }

        return true;
    }

    //------------------------------------------------------------------------------
    void save(std::ostream& stream)
    {
        stream.write(reinterpret_cast<char*>(&header), sizeof(header));
    }

    //------------------------------------------------------------------------------
  private:
    T header;
};

} // namespace COFFI

#endif // COFFI_WIN_HEADER_HPP
