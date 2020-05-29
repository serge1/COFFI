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
    class coffi : public string_to_name_provider, symbol_provider, address_provider
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
            dos_header_( 0 ),
            coff_header_( 0 ),
            optional_header_( 0 ),
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

            arch_ = COFFI_ARCH_NONE;
            dos_header_ = new dos_header;
            if ( dos_header_->load( stream ) ) {
                // It iss not EXE
                arch_ = COFFI_ARCH_PE;
            } else {
                // It was not EXE file, but it might be a PE OBJ file, or another type of COFF file
                clean();
                stream.seekg( 0 );
            }

            // Try to read a PE header
            coff_header_ = new coff_header_impl;
            if ( coff_header_->load( stream ) ) {

                // Check the machine
                static const std::vector<uint16_t> machines = {
                    IMAGE_FILE_MACHINE_AM33,
                    IMAGE_FILE_MACHINE_AMD64,
                    IMAGE_FILE_MACHINE_ARM,
                    IMAGE_FILE_MACHINE_ARMNT,
                    IMAGE_FILE_MACHINE_ARM64,
                    IMAGE_FILE_MACHINE_EBC,
                    IMAGE_FILE_MACHINE_I386,
                    IMAGE_FILE_MACHINE_IA64,
                    IMAGE_FILE_MACHINE_M32R,
                    IMAGE_FILE_MACHINE_MIPS16,
                    IMAGE_FILE_MACHINE_MIPSFPU,
                    IMAGE_FILE_MACHINE_MIPSFPU16,
                    IMAGE_FILE_MACHINE_POWERPC,
                    IMAGE_FILE_MACHINE_POWERPCFP,
                    IMAGE_FILE_MACHINE_R4000,
                    IMAGE_FILE_MACHINE_SH3,
                    IMAGE_FILE_MACHINE_SH3DSP,
                    IMAGE_FILE_MACHINE_SH4,
                    IMAGE_FILE_MACHINE_SH5,
                    IMAGE_FILE_MACHINE_THUMB,
                    IMAGE_FILE_MACHINE_WCEMIPSV2,
                };
                if (std::find(machines.begin(), machines.end(), coff_header_->get_machine()) == machines.end()) {
                    if (arch_ == COFFI_ARCH_PE) {
                        // The DOS header was detected, but the machine is not recognized
                        // This is an error
                        clean();
                        return false;
                    }
                } else {
                    // The machine is recognized, it is a PE file
                    arch_ = COFFI_ARCH_PE;
                }
            }

            // Try to read a TI header
            if (arch_ == COFFI_ARCH_NONE) {

                coff_header_ = new coff_header_impl_ti;
                stream.seekg( 0 );
                if ( !coff_header_->load( stream ) ) {
                    clean();
                    return false;
                }

                // Check the target ID
                static const std::vector<uint16_t> target_ids = {
                    TMS470,
                    TMS320C5400,
                    TMS320C6000,
                    TMS320C5500,
                    TMS320C2800,
                    MSP430,
                    TMS320C5500plus,
                };
                if (std::find(target_ids.begin(), target_ids.end(), coff_header_->get_target_id()) != target_ids.end())
                {
                    arch_ = COFFI_ARCH_TI;
                }
            }

            if (arch_ == COFFI_ARCH_NONE) {
                // The format is not recognized, process it as a PE file
                arch_ = COFFI_ARCH_PE;
                coff_header_ = new coff_header_impl;
                stream.seekg( 0 );
                if ( !coff_header_->load( stream ) ) {
                    clean();
                    return false;
                }
            }

            if ( coff_header_->get_optional_header_size() != 0 ) {
                if (arch_ == COFFI_ARCH_PE) {
                    optional_header_ = new optional_header_impl;
                }
                if (arch_ == COFFI_ARCH_TI) {
                    optional_header_ = new optional_header_impl_ti;
                }

                if ( !optional_header_->load( stream ) ) {
                    clean();
                    return false;
                }

                if (arch_ == COFFI_ARCH_PE) {
                    if ( optional_header_->get_magic() == OH_MAGIC_PE32PLUS ) {
                        win_header_ = new win_header_impl < win_headerPEPlus >;
                    }
                    else if ( optional_header_->get_magic() == OH_MAGIC_PE32 ||
                              optional_header_->get_magic() == OH_MAGIC_PE32ROM ) {
                        win_header_ = new win_header_impl < win_headerPE >;
                    }

                    if ( win_header_ != 0 ) {
                        if ( !win_header_->load( stream ) ) {
                            delete optional_header_;
                            optional_header_ = 0;
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

                if (arch_ == COFFI_ARCH_TI) {
                    // No documented optional_header_->get_magic() values
                    // Cannot check anything
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
            return dos_header_;
        }

        //---------------------------------------------------------------------
        coff_header* get_header()
        {
            return coff_header_;
        }

        //---------------------------------------------------------------------
        optional_header* get_opt_header()
        {
            return optional_header_;
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
            if ( optional_header_ != 0 &&
                 optional_header_->get_magic() == OH_MAGIC_PE32PLUS ) {
                ret = true;
            }
            return ret;
        }

        //---------------------------------------------------------------------
        int get_addressable_unit()
        {
            // returns the char type size in bytes.
            // Some targets have 2-bytes characters, this changes how offsets are computed in the file
            if (coff_header_ == 0) {
                return 0;
            }
            if (arch_ == COFFI_ARCH_TI) {
                switch (coff_header_->get_target_id()) {
                case TMS320C5400:
                case TMS320C5500:
                case TMS320C2800:
                    return 2;
                default:
                    return 1;
                }
            }
            return 1;
        }

        //---------------------------------------------------------------------
    private:
        //---------------------------------------------------------------------
        void clean()
        {
            if ( dos_header_ != 0 ) {
                delete dos_header_;
                dos_header_ = 0;
            }

            if ( coff_header_ != 0 ) {
                delete coff_header_;
                coff_header_ = 0;
            }

            if ( optional_header_ != 0 ) {
                delete optional_header_;
                optional_header_ = 0;
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
            for ( int i = 0; i < coff_header_->get_sections_count(); ++i ) {
                section* sec;
                if (arch_ == COFFI_ARCH_PE) {
                    sec = new section_impl( this, this, this );
                }
                if (arch_ == COFFI_ARCH_TI) {
                    sec = new section_impl_ti( this, this, this );
                }
                sec->load( stream, i * sec->header_sizeof() + pos );
                sec->set_index( i );
                sections_.push_back( sec );
            }

            return true;
        }

        //---------------------------------------------------------------------
        bool load_symbols( std::istream &stream )
        {
            if ( coff_header_->get_symbol_table_offset() == 0 ) {
                return true;
            }

            stream.seekg( coff_header_->get_symbol_table_offset() );
            for ( uint32_t i = 0; i < coff_header_->get_symbols_count(); ++i ) {
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
            if ( coff_header_->get_symbol_table_offset() == 0 ) {
                return true;
            }

            stream.seekg( coff_header_->get_symbol_table_offset() +
                          coff_header_->get_symbols_count() * sizeof(symbol) );
            uint32_t count;
            stream.read( reinterpret_cast<char*>( &count ), sizeof( count ) );
            strings = new char[count];
            stream.seekg( coff_header_->get_symbol_table_offset() +
                          coff_header_->get_symbols_count() * sizeof(symbol) );
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

        //---------------------------------------------------------------------
    private:
        coffi_arch_t                      arch_;
        dos_header*                       dos_header_;
        coff_header*                      coff_header_;
        optional_header*                  optional_header_;
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
