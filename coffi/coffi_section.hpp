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

#ifndef COFFI_SECTION_HPP
#define COFFI_SECTION_HPP

#include <string>
#include <iostream>

#include <coffi/coffi_utils.hpp>

namespace COFFI {

    //------------------------------------------------------------------------------
    class relocation
    {
        friend class section_impl;

    public:
        //------------------------------------------------------------------------------
        virtual ~relocation()
        {
        };

        COFFI_GET_ACCESS( uint32_t, virtual_address );
        COFFI_GET_ACCESS( uint16_t, type );

        virtual const std::string get_symbol() const
        {
            return symbol;
        }

    protected:
        //------------------------------------------------------------------------------
        rel_entry header;
        std::string symbol;
    };


    //------------------------------------------------------------------------------
    class section
    {
        friend class coffi;

    public:
        //------------------------------------------------------------------------------
        virtual ~section()
        {
        };

        COFFI_GET_ACCESS_DECL( uint32_t, index );
        COFFI_GET_ACCESS_DECL( uint32_t, virtual_size );
        COFFI_GET_ACCESS_DECL( uint32_t, virtual_address );
        COFFI_GET_ACCESS_DECL( uint32_t, data_size );
        COFFI_GET_ACCESS_DECL( uint32_t, data_offset );
        COFFI_GET_ACCESS_DECL( uint32_t, reloc_offset );
        COFFI_GET_ACCESS_DECL( uint32_t, line_num_offset );
        COFFI_GET_ACCESS_DECL( uint16_t, reloc_count );
        COFFI_GET_ACCESS_DECL( uint16_t, line_num_count );
        COFFI_GET_ACCESS_DECL( uint32_t, flags );
        COFFI_GET_ACCESS_DECL( std::string, name );
        COFFI_GET_ACCESS_DECL( const char*, data );

        virtual const std::vector<relocation>& get_relocations() = 0;

    protected:
        //------------------------------------------------------------------------------
        virtual void load( std::istream&  f,
                           std::streampos header_offset ) = 0;
        virtual void set_index( uint32_t value ) = 0;
    };


    class section_impl : public section
    {
    public:
        //------------------------------------------------------------------------------
        section_impl( string_to_name_provider* stn, symbol_provider* sym )
        {
            std::fill_n( reinterpret_cast<char*>( &header ), sizeof( header ), '\0' );
            data = 0;
            data_size = 0;
            stn_ = stn;
            sym_ = sym;
        }

        //------------------------------------------------------------------------------
        ~section_impl()
        {
            delete[] data;
        }

        //------------------------------------------------------------------------------
        // Section info functions
        COFFI_GET_ACCESS( uint32_t, virtual_size );
        COFFI_GET_ACCESS( uint32_t, virtual_address );
        COFFI_GET_ACCESS( uint32_t, data_size );
        COFFI_GET_ACCESS( uint32_t, data_offset );
        COFFI_GET_ACCESS( uint32_t, reloc_offset );
        COFFI_GET_ACCESS( uint32_t, line_num_offset );
        COFFI_GET_ACCESS( uint16_t, reloc_count );
        COFFI_GET_ACCESS( uint16_t, line_num_count );
        COFFI_GET_ACCESS( uint32_t, flags );

        //------------------------------------------------------------------------------
        uint32_t get_index() const
        {
            return index;
        }

        //------------------------------------------------------------------------------
        std::string get_name() const
        {
            return name;
        }

        //------------------------------------------------------------------------------
        const char* get_data() const
        {
            return data;
        }

        //------------------------------------------------------------------------------
        virtual const std::vector<relocation>& get_relocations()
        {
            return relocations;
        }

    protected:
        //------------------------------------------------------------------------------
        void set_index( uint32_t value )
        {
            index = value;
        }

        //------------------------------------------------------------------------------
        void load( std::istream&  stream,
                   std::streampos header_offset )
        {
            std::fill_n( reinterpret_cast<char*>( &header ), sizeof( header ), '\0' );
            stream.seekg( header_offset );
            stream.read( reinterpret_cast<char*>( &header ), sizeof( header ) );

            data_size = header.data_size;
            data = new char[data_size];
            if ( 0 != data_size ) {
                stream.seekg( header.data_offset );
                stream.read( data, data_size );
            }

            name = stn_->string_to_name( header.name );

            // Read relocations
            if ( header.reloc_count != 0 ) {
                stream.seekg( header.reloc_offset );
                for ( uint16_t i = 0; i < header.reloc_count; ++i ) {
                    relocation rel;
                    stream.read( (char*)&( rel.header ), sizeof( rel.header ) );
                    rel.symbol = stn_->string_to_name(
                        sym_->get_symbol( rel.header.symbol_table_index ).sym.name );
                    relocations.push_back( rel );
                }
            }
        }

    private:
        //------------------------------------------------------------------------------
        section_header           header;
        uint32_t                 index;
        std::string              name;
        char*                    data;
        uint32_t                 data_size;
        string_to_name_provider* stn_;
        symbol_provider*         sym_;

        std::vector<relocation> relocations;
    };

} // namespace COFFI

#endif // COFFI_SECTION_HPP
