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

#ifndef COFFI_HPP
#define COFFI_HPP

#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning(disable:4996)
//#pragma warning(disable:4355)
//#pragma warning(disable:4244)
#endif 

#include <ios>
#include <fstream>
#include <algorithm>
#include <cstring>

#include <coffi/coffi_types.hpp>
#include <coffi/coffi_utils.hpp>
#include <coffi/coffi_section.hpp>
#include <coffi/coffi_headers.hpp>

namespace COFFI {
    //-------------------------------------------------------------------------
    class coffi : public string_to_name_provider, symbol_provider
    {
    public:
        friend class Sections;
        class Sections
        {
        public:
            //------------------------------------------------------------------------------
            Sections( coffi* parent_ ) :
                parent( parent_ )
            {
            }

            //------------------------------------------------------------------------------
            size_t size() const
            {
                return parent->sections_.size();
            }

            //------------------------------------------------------------------------------
            section* operator[]( unsigned int index ) const
            {
                section* sec = 0;

                if ( index < parent->sections_.size() ) {
                    sec = parent->sections_[index];
                }

                return sec;
            }

            //------------------------------------------------------------------------------
            section* operator[]( const std::string& name ) const
            {
                section* sec = 0;

                std::vector<section*>::const_iterator it;
                for ( it = parent->sections_.begin();
                      it != parent->sections_.end();
                      ++it ) {
                    if ( ( *it )->get_name() == name ) {
                        sec = *it;
                        break;
                    }
                }

                return sec;
            }

            //------------------------------------------------------------------------------
        private:
            coffi* parent;
        };

        //---------------------------------------------------------------------
        coffi() :
            exe_header( 0 ),
            cf_header( 0 ),
            opt_header( 0 ),
            win_header_( 0 ),
            strings( 0 )
        {
        }

        //---------------------------------------------------------------------
        ~coffi()
        {
            clean();
        }

        //---------------------------------------------------------------------
        bool load( const std::string& file_name )
        {
            std::ifstream stream;
            stream.open( file_name.c_str(), std::ios::in | std::ios::binary );
            if ( !stream ) {
                return false;
            }

            return load( stream );
        }

        //---------------------------------------------------------------------
        bool load( std::istream &stream )
        {
            clean();

            stream.seekg( 0 );

            exe_header = new dos_header;
            if ( !exe_header->load( stream ) ) {
                clean();
                return false;
            }
            else {
                if ( exe_header->get_signature() != ( ( PEMAG1 << 8 ) + PEMAG0 ) ) {
                    delete exe_header;
                    exe_header = 0;
                }
            }

            cf_header = new coff_header;
            if ( !cf_header->load( stream ) ) {
                clean();
                return false;
            }

            if ( cf_header->get_optional_header_size() != 0 ) {
                opt_header = new optional_header;
                if ( !opt_header->load( stream ) ) {
                    clean();
                    return false;
                }

                if ( opt_header->get_magic() == OH_MAGIC_PE32PLUS ) {
                    win_header_ = new win_header_impl < win_headerPEPlus >;
                }
                else if ( opt_header->get_magic() == OH_MAGIC_PE32 ||
                          opt_header->get_magic() == OH_MAGIC_PE32ROM ) {
                    win_header_ = new win_header_impl < win_headerPE >;
                }

                if ( win_header_ != 0 ) {
                    if ( !win_header_->load( stream ) ) {
                        delete opt_header;
                        opt_header = 0;
                        delete win_header_;
                        win_header_ = 0;
                        clean();
                        return false;
                    }

                    for ( uint32_t i = 0; i < win_header_->get_number_of_rva_and_sizes(); ++i ) {
                        image_data_directory entry;
                        stream.read( reinterpret_cast<char*>( &entry ), sizeof( entry ) );
                        directory.push_back( entry );
                    }
                }
            }

            std::streampos pos = stream.tellg();
            if ( !load_strings( stream ) ) {
                clean();
                return false;
            }

            if ( !load_symbols( stream ) ) {
                clean();
                return false;
            }
            stream.seekg( pos );

            if ( !load_section_headers( stream ) ) {
                clean();
                return false;
            }

            return true;
        }

        //---------------------------------------------------------------------
        dos_header* get_msdos_header()
        {
            return exe_header;
        }

        //---------------------------------------------------------------------
        coff_header* get_header()
        {
            return cf_header;
        }

        //---------------------------------------------------------------------
        optional_header* get_opt_header()
        {
            return opt_header;
        }

        //---------------------------------------------------------------------
        win_header* get_win_header()
        {
            return win_header_;
        }

        const std::vector<image_data_directory>& get_directory()
        {
            return directory;
        }

        const Sections get_sections()
        {
            return Sections( this );
        }

        //---------------------------------------------------------------------
        bool is_PE32_plus()
        {
            bool ret = false;
            if ( opt_header != 0 &&
                 opt_header->get_magic() == OH_MAGIC_PE32PLUS ) {
                ret = true;
            }
            return ret;
        }

        //---------------------------------------------------------------------
    private:
        //---------------------------------------------------------------------
        void clean()
        {
            if ( exe_header != 0 ) {
                delete exe_header;
                exe_header = 0;
            }

            if ( cf_header != 0 ) {
                delete cf_header;
                cf_header = 0;
            }

            if ( opt_header != 0 ) {
                delete opt_header;
                opt_header = 0;
            }

            if ( win_header_ != 0 ) {
                delete win_header_;
                win_header_ = 0;
            }

            size_t size = sections_.size();
            for ( size_t i = 0; i < size; ++i ) {
                delete sections_[i];
            }
            sections_.clear();

            if ( strings != 0 ) {
                delete[] strings;
                strings = 0;
            }
        }

        //---------------------------------------------------------------------
        bool load_section_headers( std::istream &stream )
        {
            std::streampos pos = stream.tellg();
            for ( int i = 0; i < cf_header->get_sections_count(); ++i ) {
                section* sec = new section_impl( this, this );
                sec->load( stream, i*sizeof(section_header)+pos );
                sec->set_index( i );
                sections_.push_back( sec );
            }

            return true;
        }

        //---------------------------------------------------------------------
        bool load_symbols( std::istream &stream )
        {
            if ( cf_header->get_symbol_table_offset() == 0 ) {
                return true;
            }

            stream.seekg( cf_header->get_symbol_table_offset() );
            for ( uint32_t i = 0; i < cf_header->get_symbols_count(); ++i ) {
                symbol_ext s;

                stream.read( reinterpret_cast<char*>( &s.sym ), sizeof(symbol) );
                if ( stream.gcount() != sizeof(symbol) ) {
                    return false;
                }

                for ( uint8_t j = 0; j < s.sym.aux_symbols_number; ++j ) {
                    auxiliary_symbol_record a;
                    stream.read( reinterpret_cast<char*>( &a.value ), sizeof(symbol) );
                    if ( stream.gcount() != sizeof(symbol) ) {
                        return false;
                    }
                    s.auxs.push_back( a );
                    symbols.push_back( s );
                    ++i;
                }

                symbols.push_back( s );
            }

            return true;
        }

        //---------------------------------------------------------------------
        bool load_strings( std::istream &stream )
        {
            if ( cf_header->get_symbol_table_offset() == 0 ) {
                return true;
            }

            stream.seekg( cf_header->get_symbol_table_offset() +
                          cf_header->get_symbols_count() * sizeof(symbol) );
            uint32_t count;
            stream.read( reinterpret_cast<char*>( &count ), sizeof( count ) );
            strings = new char[count];
            stream.seekg( cf_header->get_symbol_table_offset() +
                          cf_header->get_symbols_count() * sizeof(symbol) );
            stream.read( strings, count );
            if ( stream.gcount() != static_cast<std::streamsize>(count) ) {
                return false;
            }

            return true;
        }

        //---------------------------------------------------------------------
        virtual std::string string_to_name( const char* str )
        {
            std::string ret;

            if ( *(uint32_t*)str == 0 && strings != 0 ) {
                uint32_t off = *(uint32_t*)( str + sizeof( uint32_t ) );
                ret = strings + off;
            }
            else {
                char dst[COFFI_NAME_SIZE + 1] = {'\0'};
                std::strncpy( dst, str, COFFI_NAME_SIZE );
                ret = std::string( dst );
            }

            return ret;
        }

        //---------------------------------------------------------------------
        virtual const symbol_ext get_symbol( uint32_t index )
        {
            if ( index < symbols.size() ) {
                return symbols[index];
            }

            symbol_ext sym;

            return sym;
        }

        //------------------------------------------------------------------------------
    public:

        //---------------------------------------------------------------------
    private:
        dos_header*                       exe_header;
        coff_header*                      cf_header;
        optional_header*                  opt_header;
        win_header*                       win_header_;
        std::vector<image_data_directory> directory;
        std::vector<section*>             sections_;
        std::vector<symbol_ext>           symbols;
        char*                             strings;
    };

} // namespace COFFI

#ifdef _MSC_VER
#pragma warning ( pop )
#endif

#endif //COFFI_HPP
