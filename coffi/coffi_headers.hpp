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

#ifndef COFFI_WIN_HEADER_HPP
#define COFFI_WIN_HEADER_HPP

#include <string>
#include <iostream>

#include <coffi/coffi_utils.hpp>

namespace COFFI {

    //------------------------------------------------------------------------------
    class dos_header
    {
        friend class coffi;
    public:
        //------------------------------------------------------------------------------
        dos_header()
        {
            std::fill_n( reinterpret_cast<char*>( &header ), sizeof( header ), '\0' );
        }

        //------------------------------------------------------------------------------
        COFFI_GET_ACCESS( uint16_t, signature );
        COFFI_GET_ACCESS( uint16_t, bytes_in_last_block );
        COFFI_GET_ACCESS( uint16_t, blocks_in_file );
        COFFI_GET_ACCESS( uint16_t, num_relocs );
        COFFI_GET_ACCESS( uint16_t, header_paragraphs );
        COFFI_GET_ACCESS( uint16_t, min_extra_paragraphs );
        COFFI_GET_ACCESS( uint16_t, max_extra_paragraphs );
        COFFI_GET_ACCESS( uint16_t, ss );
        COFFI_GET_ACCESS( uint16_t, sp );
        COFFI_GET_ACCESS( uint16_t, checksum );
        COFFI_GET_ACCESS( uint16_t, ip );
        COFFI_GET_ACCESS( uint16_t, cs );
        COFFI_GET_ACCESS( uint16_t, reloc_table_offset );
        COFFI_GET_ACCESS( uint16_t, overlay_number );
        COFFI_GET_ACCESS( short, oem_id );
        COFFI_GET_ACCESS( short, oem_info );
        COFFI_GET_ACCESS( long, pe_sign_location );

        //------------------------------------------------------------------------------
    protected:

        //---------------------------------------------------------------------
        virtual bool load( std::istream &stream )
        {
            // Read EXE file signature
            char e_ident0[CI_NIDENT0];
            stream.read( reinterpret_cast<char*>( &e_ident0 ), sizeof( e_ident0 ) );
            if ( stream.gcount() != sizeof( e_ident0 ) ) {
                return true;
            }

            // Is it EXE file?
            if ( e_ident0[CI_MAG0] != PEMAG0 ||
                 e_ident0[CI_MAG1] != PEMAG1 ) {
                // It was not EXE file, but it might be OBJ file
                stream.seekg( 0 );
                return true;
            }

            stream.seekg( 0 );
            stream.read( reinterpret_cast<char*>( &header ), sizeof( header ) );
            if ( stream.gcount() != sizeof( header ) ) {
                return false;
            }

            char e_ident1[CI_NIDENT1];
            stream.seekg( header.pe_sign_location );
            stream.read( reinterpret_cast<char*>( &e_ident1 ), sizeof( e_ident1 ) );
            if ( stream.gcount() != sizeof( e_ident1 ) ||
                 e_ident1[CI_MAG2] != PEMAG2 ||
                 e_ident1[CI_MAG3] != PEMAG3 ||
                 e_ident1[CI_MAG4] != PEMAG4 ||
                 e_ident1[CI_MAG5] != PEMAG5 ) {
                return false;
            }

            return true;
        }

        //------------------------------------------------------------------------------
    private:
        msdos_header header;
    };


    //------------------------------------------------------------------------------
    class coff_header
    {
        friend class coffi;
    public:
        //------------------------------------------------------------------------------
        coff_header()
        {
            std::fill_n( reinterpret_cast<char*>( &header ), sizeof( header ), '\0' );
        }

        //------------------------------------------------------------------------------
        COFFI_GET_ACCESS( uint16_t, machine );
        COFFI_GET_ACCESS( uint16_t, sections_count );
        COFFI_GET_ACCESS( uint32_t, time_data_stamp );
        COFFI_GET_ACCESS( uint32_t, symbol_table_offset );
        COFFI_GET_ACCESS( uint32_t, symbols_count );
        COFFI_GET_ACCESS( uint16_t, optional_header_size );
        COFFI_GET_ACCESS( uint16_t, flags );

        //------------------------------------------------------------------------------
    protected:

        //---------------------------------------------------------------------
        virtual bool load( std::istream &stream )
        {
            stream.read( reinterpret_cast<char*>( &header ), sizeof( header ) );
            if ( stream.gcount() != sizeof( header ) ) {
                return false;
            }

            return true;
        }

        //------------------------------------------------------------------------------
    private:
        coff_file_header header;
    };


    //------------------------------------------------------------------------------
    class optional_header
    {
        friend class coffi;
    public:
        //------------------------------------------------------------------------------
        optional_header()
        {
            std::fill_n( reinterpret_cast<char*>( &header ), sizeof( header ), '\0' );
        }

        //------------------------------------------------------------------------------
        COFFI_GET_ACCESS( uint16_t, magic );
        COFFI_GET_ACCESS( uint32_t, code_size );
        COFFI_GET_ACCESS( uint32_t, initialized_data_size );
        COFFI_GET_ACCESS( uint32_t, uninitialized_data_size );
        COFFI_GET_ACCESS( uint32_t, entry_point_address );
        COFFI_GET_ACCESS( uint32_t, code_base );
        COFFI_GET_ACCESS( uint32_t, data_base );
        COFFI_GET_ACCESS( uint8_t, major_linker_version );
        COFFI_GET_ACCESS( uint8_t, minor_linker_version );

        //------------------------------------------------------------------------------
    protected:

        //---------------------------------------------------------------------
        virtual bool load( std::istream &stream )
        {
            std::fill_n( reinterpret_cast<char*>( &header ),
                         sizeof( header ),
                         '\0' );
            stream.read( reinterpret_cast<char*>( &header ), sizeof( header ) );
            if ( stream.gcount() != sizeof( header ) ) {
                return false;
            }

            // Check magic number and decide whether we are PE32 or PE32+ format
            if ( header.magic == OH_MAGIC_PE32PLUS ) {
                // data_base field is not used for PE32+ images
                header.data_base = 0;
                stream.seekg( ( -1 ) * ( int )sizeof( header.data_base ),
                              std::ios_base::cur );
            }

            return true;
        }

        //------------------------------------------------------------------------------
    private:
        common_optional_header header;
    };


    class win_header
    {
        friend class coffi;
    public:
        virtual ~win_header()
        {
        };

        COFFI_GET_ACCESS_DECL( uint64_t, image_base );
        COFFI_GET_ACCESS_DECL( uint32_t, section_alignment );
        COFFI_GET_ACCESS_DECL( uint32_t, file_alignment );
        COFFI_GET_ACCESS_DECL( uint16_t, major_os_version );
        COFFI_GET_ACCESS_DECL( uint16_t, minor_os_version );
        COFFI_GET_ACCESS_DECL( uint16_t, major_image_version );
        COFFI_GET_ACCESS_DECL( uint16_t, minor_image_version );
        COFFI_GET_ACCESS_DECL( uint16_t, major_subsystem_version );
        COFFI_GET_ACCESS_DECL( uint16_t, minor_subsystem_version );
        COFFI_GET_ACCESS_DECL( uint32_t, win32_version_value );
        COFFI_GET_ACCESS_DECL( uint32_t, image_size );
        COFFI_GET_ACCESS_DECL( uint32_t, headers_size );
        COFFI_GET_ACCESS_DECL( uint32_t, checksum );
        COFFI_GET_ACCESS_DECL( uint16_t, subsystem );
        COFFI_GET_ACCESS_DECL( uint16_t, dll_flags );
        COFFI_GET_ACCESS_DECL( uint64_t, stack_reserve_size );
        COFFI_GET_ACCESS_DECL( uint64_t, stack_commit_size );
        COFFI_GET_ACCESS_DECL( uint64_t, heap_reserve_size );
        COFFI_GET_ACCESS_DECL( uint64_t, heap_commit_size );
        COFFI_GET_ACCESS_DECL( uint32_t, loader_flags );
        COFFI_GET_ACCESS_DECL( uint32_t, number_of_rva_and_sizes );

    protected:
        virtual bool load( std::istream&  f ) = 0;
    };


    template< class T >
    class win_header_impl : public win_header
    {
    public:
        //------------------------------------------------------------------------------
        win_header_impl()
        {
            std::fill_n( reinterpret_cast<char*>( &header ), sizeof( header ), '\0' );
        }

        //------------------------------------------------------------------------------
        COFFI_GET_ACCESS( uint64_t, image_base );
        COFFI_GET_ACCESS( uint32_t, section_alignment );
        COFFI_GET_ACCESS( uint32_t, file_alignment );
        COFFI_GET_ACCESS( uint16_t, major_os_version );
        COFFI_GET_ACCESS( uint16_t, minor_os_version );
        COFFI_GET_ACCESS( uint16_t, major_image_version );
        COFFI_GET_ACCESS( uint16_t, minor_image_version );
        COFFI_GET_ACCESS( uint16_t, major_subsystem_version );
        COFFI_GET_ACCESS( uint16_t, minor_subsystem_version );
        COFFI_GET_ACCESS( uint32_t, win32_version_value );
        COFFI_GET_ACCESS( uint32_t, image_size );
        COFFI_GET_ACCESS( uint32_t, headers_size );
        COFFI_GET_ACCESS( uint32_t, checksum );
        COFFI_GET_ACCESS( uint16_t, subsystem );
        COFFI_GET_ACCESS( uint16_t, dll_flags );
        COFFI_GET_ACCESS( uint64_t, stack_reserve_size );
        COFFI_GET_ACCESS( uint64_t, stack_commit_size );
        COFFI_GET_ACCESS( uint64_t, heap_reserve_size );
        COFFI_GET_ACCESS( uint64_t, heap_commit_size );
        COFFI_GET_ACCESS( uint32_t, loader_flags );
        COFFI_GET_ACCESS( uint32_t, number_of_rva_and_sizes );

        //------------------------------------------------------------------------------
    protected:
        virtual bool load( std::istream&  stream )
        {
            int read_size = sizeof( header );
            std::fill_n( reinterpret_cast<char*>( &header ),
                         read_size,
                         '\0' );
            stream.read( reinterpret_cast<char*>( &header ), read_size );
            if ( stream.gcount() != read_size ) {
                return false;
            }

            return true;
        }

        //------------------------------------------------------------------------------
    private:
        T header;
    };

} // namespace COFFI

#endif // COFFI_WIN_HEADER_HPP
