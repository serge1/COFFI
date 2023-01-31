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

/*! @file coffi_section.hpp
 * @brief COFFI library classes for the COFF sections.
 *
 * Do not include this file directly. This file is included by coffi.hpp.
 */

#ifndef COFFI_SECTION_HPP
#define COFFI_SECTION_HPP

#include <string>
#include <iostream>

#include <coffi/coffi_utils.hpp>
#include <coffi/coffi_relocation.hpp>

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
//! Interface class for accessing a COFF section, for all the COFF architectures.
class section
{
  public:
    //------------------------------------------------------------------------------
    virtual ~section(){};

    //! @accessors{section}
    COFFI_GET_SET_ACCESS_DECL(uint32_t, index);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, virtual_size);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, physical_address);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, virtual_address);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, data_size);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, data_offset);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, reloc_offset);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, line_num_offset);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, reloc_count);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, line_num_count);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, flags);
    COFFI_GET_SET_ACCESS_DECL(uint16_t, page_number);
    COFFI_GET_SET_ACCESS_DECL(uint32_t, alignment);

    COFFI_GET_SIZEOF_DECL();
    //! @endaccessors

    virtual const std::string& get_name() const                             = 0;
    virtual void               set_name(const std::string& name)            = 0;
    virtual const char*        get_data() const                             = 0;
    virtual void               set_data(const char* data, uint32_t size)    = 0;
    virtual void               set_data(const std::string& data)            = 0;
    virtual void               append_data(const char* data, uint32_t size) = 0;
    virtual void               append_data(const std::string& data)         = 0;

    //------------------------------------------------------------------------------
    virtual bool load(std::istream& stream, std::streampos header_offset) = 0;
    virtual void save_header(std::ostream& stream)                        = 0;
    virtual void save_data(std::ostream& stream)                          = 0;
    virtual void save_relocations(std::ostream& stream)                   = 0;
    virtual uint32_t get_relocations_filesize()                           = 0;
    virtual void     save_line_numbers(std::ostream& stream)              = 0;
    virtual uint32_t get_line_numbers_filesize()                          = 0;

    //------------------------------------------------------------------------------
    virtual const std::vector<relocation>& get_relocations() const    = 0;
    virtual void add_relocation_entry(const rel_entry_generic* entry) = 0;
};

/*! @brief Template class for accessing a COFF section, depends on the underlying section header format.
     *
     * The template parameter **class T** is one of:
     *   - section_header:    @copybrief section_header
     *   - section_header_ti: @copybrief section_header_ti
     */
template <class T> class section_impl_tmpl : public section
{
  public:
    //------------------------------------------------------------------------------
    section_impl_tmpl(string_to_name_provider* stn,
                      symbol_provider*         sym,
                      architecture_provider*   arch)
    {
        std::fill_n(reinterpret_cast<char*>(&header), sizeof(header), '\0');
        data_          = 0;
        data_reserved_ = 0;
        stn_           = stn;
        sym_           = sym;
        arch_          = arch;
    }

    //------------------------------------------------------------------------------
    //! Discards the copy constructor
    section_impl_tmpl(const section_impl_tmpl&) = delete;

    //------------------------------------------------------------------------------
    ~section_impl_tmpl() { clean(); }

    //------------------------------------------------------------------------------
    // Section info functions

    //! @accessors{section_impl_tmpl}
    COFFI_GET_SET_ACCESS(uint32_t, virtual_address);
    COFFI_GET_SET_ACCESS(uint32_t, data_offset);
    COFFI_GET_SET_ACCESS(uint32_t, reloc_offset);
    COFFI_GET_SET_ACCESS(uint32_t, reloc_count);
    COFFI_GET_SET_ACCESS(uint32_t, line_num_count);
    COFFI_GET_SET_ACCESS(uint32_t, flags);

    COFFI_GET_SIZEOF();
    //! @endaccessors

    //------------------------------------------------------------------------------
    uint32_t get_index() const { return index; }

    //------------------------------------------------------------------------------
    void set_index(uint32_t value) { index = value; }

    //------------------------------------------------------------------------------
    const std::string& get_name() const { return name; }

    //------------------------------------------------------------------------------
    void set_name(const std::string& value)
    {
        stn_->name_to_section_string(value, header.name);
    }

    //------------------------------------------------------------------------------
    const char* get_data() const { return data_; }

    //------------------------------------------------------------------------------
    void set_data(const char* data, uint32_t size)
    {
        clean();
        if (!data) {
            data_reserved_ = 0;
        }
        else {
            data_ = new char[size];
            if (data_) {
                data_reserved_ = size;
                std::copy(data, data + size, data_);
            }
            else {
                data_reserved_ = 0;
            }
        }
        set_data_size(data_reserved_);
    }

    //------------------------------------------------------------------------------
    void set_data(const std::string& data)
    {
        set_data(data.c_str(), (uint32_t)data.size());
    }

    //------------------------------------------------------------------------------
    virtual void append_data(const char* data, uint32_t size)
    {
        if (!data_) {
            set_data(data, size);
        }
        if (get_data_size() + size <= data_reserved_) {
            std::copy(data, data + size, data_ + get_data_size());
        }
        else {
            uint32_t new_data_size = 2 * (data_reserved_ + size);
            char*    new_data      = new char[new_data_size];
            if (!new_data) {
                size = 0;
            }
            else {
                data_reserved_ = new_data_size;
                std::copy(data_, data_ + get_data_size(), new_data);
                std::copy(data, data + size, new_data + get_data_size());
                delete[] data_;
                data_ = new_data;
            }
        }
        set_data_size(get_data_size() + size);
    }

    //------------------------------------------------------------------------------
    virtual void append_data(const std::string& data)
    {
        append_data(data.c_str(), (uint32_t)data.size());
    }

    //------------------------------------------------------------------------------
    virtual const std::vector<relocation>& get_relocations() const
    {
        return relocations;
    }

    //------------------------------------------------------------------------------
    virtual void add_relocation_entry(const rel_entry_generic* entry)
    {
        relocation r{stn_, sym_, arch_};
        r.set_type(entry->type);
        r.set_virtual_address(entry->virtual_address);
        r.set_symbol(entry->symbol_table_index);
        relocations.push_back(r);
        set_reloc_count(narrow_cast<uint32_t>(relocations.size()));
    }

    //------------------------------------------------------------------------------
    bool load(std::istream& stream, std::streampos header_offset)
    {
        std::fill_n(reinterpret_cast<char*>(&header), sizeof(header), '\0');
        stream.seekg(header_offset);
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));

        name = stn_->section_string_to_name(header.name);

        // Read the raw section data
        bool dont = (arch_->get_architecture() == COFFI_ARCHITECTURE_CEVA) &&
                    ((get_flags() & CEVA_UNINITIALIZED_DATA) ==
                     CEVA_UNINITIALIZED_DATA);
        if (!dont) {
            data_reserved_ = get_data_size();
            if ((get_data_offset() != 0) && (data_reserved_ != 0)) {
                data_ = new char[data_reserved_];
                if (!data_) {
                    return false;
                }
                stream.seekg(get_data_offset());
                stream.read(data_, data_reserved_);
            }
        }

        // Read relocations
        if (get_reloc_count() != 0) {
            stream.seekg(get_reloc_offset());
            for (uint32_t i = 0; i < get_reloc_count(); ++i) {
                relocation rel{stn_, sym_, arch_};
                rel.load(stream);
                relocations.push_back(rel);
            }
        }

        // Read line numbers
        if (get_line_num_count() != 0) {
            stream.seekg(get_line_num_offset());
            for (uint32_t i = 0; i < get_line_num_count(); ++i) {
                line_number lnum;
                stream.read(reinterpret_cast<char*>(&lnum),
                            sizeof(line_number));
                line_numbers.push_back(lnum);
            }
        }
        return true;
    }

    //------------------------------------------------------------------------------
    virtual void save_header(std::ostream& stream)
    {
        stream.write(reinterpret_cast<char*>(&header), sizeof(header));
    }

    //------------------------------------------------------------------------------
    virtual void save_data(std::ostream& stream)
    {
        if (!data_ || get_data_offset() == 0) {
            return;
        }
        stream.write(data_, get_data_size());
    }

    //------------------------------------------------------------------------------
    virtual void save_relocations(std::ostream& stream)
    {
        for (auto entry : relocations) {
            entry.save(stream);
        }
    }

    //------------------------------------------------------------------------------
    virtual uint32_t get_relocations_filesize()
    {
        relocation rel{stn_, sym_, arch_};
        return rel.get_sizeof() * narrow_cast<uint32_t>(relocations.size());
    }

    //------------------------------------------------------------------------------
    virtual void save_line_numbers(std::ostream& stream)
    {
        for (auto lnum : line_numbers) {
            stream.write(reinterpret_cast<char*>(&lnum), sizeof(line_number));
        }
    }

    //------------------------------------------------------------------------------
    virtual uint32_t get_line_numbers_filesize()
    {
        return sizeof(line_number) * narrow_cast<uint32_t>(line_numbers.size());
    }

    //------------------------------------------------------------------------------
  protected:
    //------------------------------------------------------------------------------
    void clean()
    {
        if (data_) {
            delete[] data_;
            data_          = 0;
            data_reserved_ = 0;
        }
    }

    //------------------------------------------------------------------------------
    uint32_t get_bit_number(uint32_t v)
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        for (uint32_t i = 0; i < 32; i++) {
            if (v & 1)
                return i;
            v = v >> 1;
        }
        return 32;
    }

    //------------------------------------------------------------------------------
    T                        header;
    uint32_t                 index;
    std::string              name;
    char*                    data_;
    uint32_t                 data_reserved_;
    string_to_name_provider* stn_;
    symbol_provider*         sym_;
    architecture_provider*   arch_;

    std::vector<relocation>  relocations;
    std::vector<line_number> line_numbers;
};

//------------------------------------------------------------------------------
//! Class for accessing a COFF section, for the PE format.
class section_impl : public section_impl_tmpl<section_header>
{
  public:
    //------------------------------------------------------------------------------
    section_impl(string_to_name_provider* stn,
                 symbol_provider*         sym,
                 architecture_provider*   arch)
        : section_impl_tmpl{stn, sym, arch}
    {
    }

    //! @accessors{section_impl}
    COFFI_GET_SET_ACCESS(uint32_t, virtual_size);
    COFFI_GET_SET_ACCESS(uint32_t, data_size);
    COFFI_GET_SET_ACCESS_NONE(uint32_t, physical_address);
    COFFI_GET_SET_ACCESS(uint32_t, line_num_offset);
    COFFI_GET_SET_ACCESS_NONE(uint16_t, page_number);
    //! @endaccessors

    //------------------------------------------------------------------------------
    uint32_t get_alignment() const
    {
        return 1 << (((get_flags() >> 20) & 0xF) - 1);
    }

    //------------------------------------------------------------------------------
    void set_alignment(uint32_t value)
    {
        set_flags((get_flags() & ~0xF00000) |
                  ((get_bit_number(value) & 0xF) << 20));
    }
};

//------------------------------------------------------------------------------
//! Class for accessing a COFF section, for the Texas Instruments format.
class section_impl_ti : public section_impl_tmpl<section_header_ti>
{
  public:
    //------------------------------------------------------------------------------
    section_impl_ti(string_to_name_provider* stn,
                    symbol_provider*         sym,
                    architecture_provider*   arch)
        : section_impl_tmpl{stn, sym, arch}
    {
    }

    //! @accessors{section_impl_ti}
    COFFI_GET_SET_ACCESS_NONE(uint32_t, virtual_size);
    COFFI_GET_SET_ACCESS(uint32_t, physical_address);
    COFFI_GET_SET_ACCESS_NONE(uint32_t, line_num_offset);
    COFFI_GET_SET_ACCESS(uint16_t, page_number);
    //! @endaccessors

    //------------------------------------------------------------------------------
    uint32_t get_data_size() const
    {
        auto sec_type     = get_flags() & 0x1F;
        bool is_allocated = (sec_type == STYP_REG) || (sec_type == STYP_NOLOAD);
        if (is_allocated) {
            return header.data_size * arch_->get_addressable_unit();
        }
        else {
            return header.data_size;
        }
    }

    //------------------------------------------------------------------------------
    void set_data_size(uint32_t value)
    {
        auto sec_type     = get_flags() & 0x1F;
        bool is_allocated = (sec_type == STYP_REG) || (sec_type == STYP_NOLOAD);
        if (is_allocated) {
            header.data_size = value / arch_->get_addressable_unit();
        }
        else {
            header.data_size = value;
        }
    }

    //------------------------------------------------------------------------------
    uint32_t get_alignment() const { return 1 << ((get_flags() >> 8) & 0xF); }

    //------------------------------------------------------------------------------
    void set_alignment(uint32_t value)
    {
        set_flags((get_flags() & ~0xF00) |
                  ((get_bit_number(value) & 0xF) << 8));
    }
};

/*! @brief List of sections
     *
     * It is implemented as a vector of @ref section pointers.
     * This allows to manage easily all the different section implementations (for every COFF format),
     * with pointers to their base interface class (@ref section).
     */
//------------------------------------------------------------------------------
class sections : public std::vector<section*>
{
  public:
    //------------------------------------------------------------------------------
    sections() {}

    //------------------------------------------------------------------------------
    //! Discards the copy constructor
    sections(const sections&) = delete;

    //------------------------------------------------------------------------------
    virtual ~sections() { clean(); }

    //------------------------------------------------------------------------------
    void clean()
    {
        for (auto sec : *this) {
            delete sec;
        }
        clear();
    }

    //------------------------------------------------------------------------------
    /*! @brief Subscript operator, finds a section by its index
         *
         * @param[in] i Index of the section
         * @return A reference to the element at specified location **i**
         */
    section* operator[](size_t i)
    {
        return std::vector<section*>::operator[](i);
    }

    //------------------------------------------------------------------------------
    /*! @copydoc operator[](size_t)
         */
    const section* operator[](size_t i) const
    {
        return std::vector<section*>::operator[](i);
    }

    //------------------------------------------------------------------------------
    /*! @brief Subscript operator, finds a section by its symbolic name
         *
         * @param[in] name Symbolic name of the section
         * @return A reference to the element with the section symbolic name **name**
         */
    section* operator[](const std::string& name)
    {
        return (section*)((const sections*)this)->operator[](name);
    }

    //------------------------------------------------------------------------------
    /*! @copydoc operator[](const std::string&)
         */
    const section* operator[](const std::string& name) const
    {
        for (section* sec : *this) {
            if (sec->get_name() == name) {
                return sec;
            }
        }
        return 0;
    }
};

} // namespace COFFI

#endif // COFFI_SECTION_HPP
