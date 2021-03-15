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

/*! @file coffi_relocation.hpp 
 * @brief COFFI library classes for the COFF relocation entries.
 *
 * Do not include this file directly. This file is included by coffi.hpp.
 */

#ifndef COFFI_RELOCATION_HPP
#define COFFI_RELOCATION_HPP

#include <string>
#include <iostream>

#include <coffi/coffi_utils.hpp>
#include <coffi/coffi_symbols.hpp>

namespace COFFI {

//------------------------------------------------------------------------------
//! Class for accessing a COFF section relocation entry.
class relocation
{
  public:
    //------------------------------------------------------------------------------
    relocation(const string_to_name_provider* stn,
               const symbol_provider*         sym,
               const architecture_provider*   arch)
        : stn_{stn}, sym_{sym}, arch_{arch}
    {
    }

    //! @accessors{relocation}
    COFFI_GET_SET_ACCESS(uint32_t, virtual_address);
    COFFI_GET_ACCESS(uint32_t, symbol_table_index);
    COFFI_GET_SET_ACCESS(uint32_t, type);
    COFFI_GET_SET_ACCESS(uint16_t, reserved);
    //! @endaccessors

    //------------------------------------------------------------------------------
    const std::string& get_symbol() const { return symbol_name; }

    //------------------------------------------------------------------------------
    void set_symbol(uint32_t symbol_table_index)
    {
        header.symbol_table_index = symbol_table_index;
        const symbol* sym         = sym_->get_symbol(header.symbol_table_index);
        symbol_name               = "";
        if (sym) {
            symbol_name = sym->get_name();
        }
    }

    //------------------------------------------------------------------------------
    void load(std::istream& stream)
    {
        std::fill_n(reinterpret_cast<char*>(&header), sizeof(header), '\0');

        switch (arch_->get_architecture()) {
        case COFFI_ARCHITECTURE_TI:
        {
            rel_entry_ti h;
            stream.read((char*)&(h), sizeof(h));
            header.virtual_address    = h.virtual_address;
            header.symbol_table_index = h.symbol_table_index;
            header.type               = h.type;
            header.reserved           = h.reserved;
            break;
        }
        case COFFI_ARCHITECTURE_CEVA:
        {
            rel_entry_ceva h;
            stream.read((char*)&(h), sizeof(h));
            header.virtual_address    = h.virtual_address;
            header.symbol_table_index = h.symbol_table_index;
            header.type               = h.type;
            break;
        }
        default:
        {
            rel_entry h;
            stream.read((char*)&(h), sizeof(h));
            header.virtual_address    = h.virtual_address;
            header.symbol_table_index = h.symbol_table_index;
            header.type               = h.type;
            break;
        }
        }

        set_symbol(get_symbol_table_index());
    }

    //------------------------------------------------------------------------------
    void save(std::ostream& stream)
    {
        switch (arch_->get_architecture()) {
        case COFFI_ARCHITECTURE_TI:
        {
            rel_entry_ti h;
            h.virtual_address    = header.virtual_address;
            h.symbol_table_index = header.symbol_table_index;
            h.type               = header.type;
            h.reserved           = header.reserved;
            stream.write((char*)&(h), sizeof(h));
            break;
        }
        case COFFI_ARCHITECTURE_CEVA:
        {
            rel_entry_ceva h;
            h.virtual_address    = header.virtual_address;
            h.symbol_table_index = header.symbol_table_index;
            h.type               = header.type;
            stream.write((char*)&(h), sizeof(h));
            break;
        }
        default:
        {
            rel_entry h;
            h.virtual_address    = header.virtual_address;
            h.symbol_table_index = header.symbol_table_index;
            h.type               = header.type;
            stream.write((char*)&(h), sizeof(h));
            break;
        }
        }
    }

    //------------------------------------------------------------------------------
    uint32_t get_sizeof() const
    {
        switch (arch_->get_architecture()) {
        case COFFI_ARCHITECTURE_TI:
            return sizeof(rel_entry_ti);
            break;
        case COFFI_ARCHITECTURE_CEVA:
            return sizeof(rel_entry_ceva);
            break;
        default:
            return sizeof(rel_entry);
            break;
        }
    }

    //------------------------------------------------------------------------------
  protected:
    const string_to_name_provider* stn_;
    const symbol_provider*         sym_;
    const architecture_provider*   arch_;
    std::string                    symbol_name;
    rel_entry_generic              header;
};

} // namespace COFFI

#endif // COFFI_RELOCATION_HPP
