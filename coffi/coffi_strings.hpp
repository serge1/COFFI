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

/*! @file coffi_strings.hpp 
 * @brief COFFI library classes for the COFF strings and string table.
 *
 * Do not include this file directly. This file is included by coffi.hpp.
 */

#ifndef COFFI_STRINGS_HPP
#define COFFI_STRINGS_HPP

#include <cstring>

#include <coffi/coffi_utils.hpp>
#include <coffi/coffi_headers.hpp>

namespace COFFI {
//-------------------------------------------------------------------------
//! Class for accessing the strings table.
class coffi_strings : public virtual string_to_name_provider
{
  public:
    //---------------------------------------------------------------------
    coffi_strings() : strings_{0}, strings_reserved_{0} { clean_strings(); }

    //---------------------------------------------------------------------
    //! Discards the copy constructor
    coffi_strings(const coffi_strings&) = delete;

    //---------------------------------------------------------------------
    virtual ~coffi_strings()
    {
        clean_strings();
        delete[] strings_;
    }

    //---------------------------------------------------------------------
    uint32_t get_strings_size() const
    {
        if (!strings_) {
            return 0;
        }
        return *reinterpret_cast<uint32_t*>(strings_);
    }

    //---------------------------------------------------------------------
    void set_strings_size(uint32_t value)
    {
        if (!strings_) {
            return;
        }
        *reinterpret_cast<uint32_t*>(strings_) = value;
    }

    //---------------------------------------------------------------------
    //! @copydoc string_to_name_provider::string_to_name()
    virtual std::string string_to_name(const char* str) const
    {
        return string_to_name_internal(str, false);
    }

    //---------------------------------------------------------------------
    //! @copydoc string_to_name_provider::section_string_to_name()
    virtual std::string section_string_to_name(const char* str) const
    {
        return string_to_name_internal(str, true);
    }

    //---------------------------------------------------------------------
    //! @copydoc string_to_name_provider::name_to_string()
    virtual void name_to_string(const std::string& name, char* str)
    {
        return name_to_string_internal(name, str, false);
    }

    //---------------------------------------------------------------------
    //! @copydoc string_to_name_provider::name_to_section_string()
    virtual void name_to_section_string(const std::string& name, char* str)
    {
        return name_to_string_internal(name, str, true);
    }

    //---------------------------------------------------------------------
    virtual const char* get_strings() const { return strings_; }

    //---------------------------------------------------------------------
    virtual void set_strings(const char* str, uint32_t size)
    {
        char* new_strings = new char[size];
        if (new_strings) {
            std::copy(str, str + size, new_strings);
            delete[] strings_;
            strings_          = new_strings;
            strings_reserved_ = size;
            set_strings_size(size);
        }
    }

    //---------------------------------------------------------------------
  protected:
    //---------------------------------------------------------------------
    void clean_strings()
    {
        if (strings_) {
            delete[] strings_;
        }
        strings_          = new char[4];
        strings_reserved_ = 4;
        set_strings_size(4);
    }

    //---------------------------------------------------------------------
    bool load_strings(std::istream& stream, const coff_header* header)
    {
        clean_strings();

        if (header->get_symbol_table_offset() == 0) {
            return true;
        }

        uint32_t strings_offset =
            header->get_symbol_table_offset() +
            header->get_symbols_count() * sizeof(symbol_record);
        stream.seekg(strings_offset);
        stream.read(strings_, 4);
        char* new_strings = new char[get_strings_size()];
        if (!new_strings) {
            return false;
        }
        strings_reserved_ = get_strings_size();
        stream.seekg(strings_offset);
        stream.read(new_strings, get_strings_size());
        if (stream.gcount() !=
            static_cast<std::streamsize>(get_strings_size())) {
            return false;
        }
        delete[] strings_;
        strings_ = new_strings;
        return true;
    }

    //---------------------------------------------------------------------
    void save_strings(std::ostream& stream) const
    {
        if (strings_ && get_strings_size() > 4) {
            stream.write(strings_, get_strings_size());
        }
    }

    //---------------------------------------------------------------------
    virtual std::string string_to_name_internal(const char* str,
                                                bool        is_section) const
    {
        std::string ret;

        if (*(uint32_t*)str == 0 && strings_) {
            uint32_t off = *(uint32_t*)(str + sizeof(uint32_t));
            ret          = strings_ + off;
        }
        else if (is_section && str[0] == '/') {
            int32_t off = std::atol(str + 1);
            ret         = strings_ + off;
        }
        else {
            char dst[COFFI_NAME_SIZE + 1];
            dst[COFFI_NAME_SIZE] = 0;
            std::strncpy(dst, str, COFFI_NAME_SIZE);
            ret = std::string(dst);
        }

        return ret;
    }

    //---------------------------------------------------------------------
    virtual void
    name_to_string_internal(const std::string& name, char* str, bool is_section)
    {
        size_t size = name.size();
        if (size > COFFI_NAME_SIZE) {
            size++;
            uint32_t offset = get_strings_size();
            if (get_strings_size() + size > strings_reserved_) {
                uint32_t new_strings_reserved =
                    2 * (strings_reserved_ + narrow_cast<uint32_t>(size));
                char* new_strings = new char[new_strings_reserved];
                if (!new_strings) {
                    offset = 0;
                    size   = 0;
                }
                else {
                    strings_reserved_ = new_strings_reserved;
                    std::copy(strings_, strings_ + get_strings_size(),
                              new_strings);
                    delete[] strings_;
                    strings_ = new_strings;
                }
            }
            std::copy(name.c_str(), name.c_str() + size,
                      strings_ + get_strings_size());
            set_strings_size(get_strings_size() + narrow_cast<uint32_t>(size));
            if (is_section) {
                str[0]        = '/';
                std::string s = std::to_string(offset);
                std::strncpy(str + 1, s.c_str(), COFFI_NAME_SIZE - 1);
            }
            else {
                *(uint32_t*)str       = 0;
                *((uint32_t*)str + 1) = offset;
            }
        }
        else {
            std::fill_n(str, COFFI_NAME_SIZE, '\0');
            std::copy(name.begin(), name.end(), str);
        }
    }

    //---------------------------------------------------------------------
    char*    strings_;
    uint32_t strings_reserved_;
};

} // namespace COFFI

#endif //COFFI_STRINGS_HPP
