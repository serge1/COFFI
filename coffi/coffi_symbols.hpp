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

/*! @file coffi_symbols.hpp
 * @brief COFFI library classes for the COFF symbols and symbol table.
 *
 * Do not include this file directly. This file is included by coffi.hpp.
 */

#ifndef COFFI_SYMBOLS_HPP
#define COFFI_SYMBOLS_HPP

#include <vector>

#include <coffi/coffi_utils.hpp>
#include <coffi/coffi_headers.hpp>

#if defined(__has_include) && __has_include(<gsl/narrow>)
#include <gsl/narrow>
using gsl::narrow_cast;
#else
#ifndef narrow_cast
#define narrow_cast static_cast
#endif
#endif

namespace COFFI {

//-------------------------------------------------------------------------
//! Class for accessing a COFF symbol.
class symbol
{
  public:
    //---------------------------------------------------------------------
    symbol(string_to_name_provider* stn) : index_(0), stn_{stn}
    {
        std::fill_n(reinterpret_cast<char*>(&header), sizeof(header), '\0');
    }

    //---------------------------------------------------------------------
    //! @accessors{symbol}
    COFFI_GET_SET_ACCESS(uint32_t, value);
    COFFI_GET_SET_ACCESS(uint16_t, section_number);
    COFFI_GET_SET_ACCESS(uint16_t, type);
    COFFI_GET_SET_ACCESS(uint8_t, storage_class);
    COFFI_GET_SET_ACCESS(uint8_t, aux_symbols_number);
    //! @endaccessors

    //---------------------------------------------------------------------
    uint32_t get_index() const { return index_; }

    //---------------------------------------------------------------------
    void set_index(uint32_t index) { index_ = index; }

    //------------------------------------------------------------------------------
    const std::string get_name() const
    {
        return stn_->string_to_name(header.name);
    }

    //---------------------------------------------------------------------
    void set_name(const std::string& value)
    {
        stn_->name_to_string(value, header.name);
    }

    //---------------------------------------------------------------------
    const std::vector<auxiliary_symbol_record>& get_auxiliary_symbols() const
    {
        return auxs;
    }

    //---------------------------------------------------------------------
    std::vector<auxiliary_symbol_record>& get_auxiliary_symbols()
    {
        return auxs;
    }

    //---------------------------------------------------------------------
    bool load(std::istream& stream)
    {
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (stream.gcount() != sizeof(header)) {
            return false;
        }

        for (uint8_t i = 0; i < get_aux_symbols_number(); ++i) {
            auxiliary_symbol_record a;
            stream.read(reinterpret_cast<char*>(&a), sizeof(symbol_record));
            if (stream.gcount() != sizeof(symbol_record)) {
                return false;
            }
            auxs.push_back(a);
        }
        return true;
    }

    //---------------------------------------------------------------------
    void save(std::ostream& stream)
    {
        set_aux_symbols_number(narrow_cast<uint8_t>(auxs.size()));
        stream.write(reinterpret_cast<char*>(&header), sizeof(header));
        for (auto aux : auxs) {
            stream.write(reinterpret_cast<char*>(&aux), sizeof(symbol_record));
        }
    }

    //---------------------------------------------------------------------
  protected:
    symbol_record                        header;
    std::vector<auxiliary_symbol_record> auxs;
    uint32_t                             index_;
    string_to_name_provider*             stn_;
};

//-------------------------------------------------------------------------
//! Class for accessing the symbol table.
class coffi_symbols : public virtual symbol_provider,
                      public virtual string_to_name_provider
{
  public:
    //---------------------------------------------------------------------
    coffi_symbols() {}

    //---------------------------------------------------------------------
    ~coffi_symbols() { clean_symbols(); }

    //---------------------------------------------------------------------
    //! @copydoc symbol_provider::get_symbol(uint32_t)
    virtual symbol* get_symbol(uint32_t index)
    {
        return (symbol*)((const coffi_symbols*)this)->get_symbol(index);
    }

    //---------------------------------------------------------------------
    //! @copydoc symbol_provider::get_symbol(uint32_t)
    virtual const symbol* get_symbol(uint32_t index) const
    {
        uint32_t L = 0;
        uint32_t R = narrow_cast<uint32_t>(symbols_.size()) - 1;
        while (L <= R) {
            uint32_t m = (L + R) / 2;
            if (symbols_[m].get_index() < index) {
                L = m + 1;
            }
            else if (symbols_[m].get_index() > index) {
                R = m - 1;
            }
            else {
                return &(symbols_[m]);
            }
        }
        return nullptr;
    }

    //---------------------------------------------------------------------
    //! @copydoc symbol_provider::get_symbol(const std::string &)
    virtual symbol* get_symbol(const std::string& name)
    {
        return (symbol*)((const coffi_symbols*)this)->get_symbol(name);
    }

    //---------------------------------------------------------------------
    //! @copydoc symbol_provider::get_symbol(const std::string &)
    virtual const symbol* get_symbol(const std::string& name) const
    {
        for (auto s = symbols_.begin(); s != symbols_.end(); s++) {
            if (s->get_name() == name) {
                return &(*s);
            }
        }
        return nullptr;
    }

    //---------------------------------------------------------------------
    std::vector<symbol>* get_symbols() { return &symbols_; }

    //---------------------------------------------------------------------
    const std::vector<symbol>* get_symbols() const { return &symbols_; }

    //---------------------------------------------------------------------
    //! @copydoc symbol_provider::add_symbol()
    symbol* add_symbol(const std::string& name)
    {
        uint32_t index = 0;
        if (symbols_.size() > 0) {
            index = (symbols_.end() - 1)->get_index() + 1 +
                    narrow_cast<uint32_t>(
                        (symbols_.end() - 1)->get_auxiliary_symbols().size());
        }
        symbol s{this};
        s.set_index(index);
        s.set_name(name);
        symbols_.push_back(s);
        return &*(symbols_.end() - 1);
    }

    //---------------------------------------------------------------------
  protected:
    //---------------------------------------------------------------------
    void clean_symbols() { symbols_.clear(); }

    //---------------------------------------------------------------------
    bool load_symbols(std::istream& stream, const coff_header* header)
    {
        if (header->get_symbol_table_offset() == 0) {
            return true;
        }

        stream.seekg(header->get_symbol_table_offset());
        for (uint32_t i = 0; i < header->get_symbols_count(); ++i) {
            symbol s{this};
            if (!s.load(stream)) {
                return false;
            }
            s.set_index(i);
            i += narrow_cast<uint32_t>(s.get_auxiliary_symbols().size());
            symbols_.push_back(s);
        }

        return true;
    }

    //---------------------------------------------------------------------
    void save_symbols(std::ostream& stream)
    {
        for (auto s : symbols_) {
            s.save(stream);
        }
    }

    //---------------------------------------------------------------------
    uint32_t get_symbols_filesize()
    {
        uint32_t filesize = 0;
        for (auto s : symbols_) {
            filesize +=
                sizeof(symbol_record) *
                (1 + narrow_cast<uint32_t>(s.get_auxiliary_symbols().size()));
        }
        return filesize;
    }

    //---------------------------------------------------------------------
    std::vector<symbol> symbols_;
};

} // namespace COFFI

#endif //COFFI_SYMBOLS_HPP
