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

/*! @file coffi.hpp 
 * @brief The COFFI library include file
 *
 * The only file required to include when using the COFFI library.
 * It includes the other header files.
 */

#ifndef COFFI_HPP
#define COFFI_HPP

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
//#pragma warning(disable:4355)
//#pragma warning(disable:4244)
#endif

#include <ios>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <vector>

#include <coffi/coffi_types.hpp>
#include <coffi/coffi_utils.hpp>
#include <coffi/coffi_section.hpp>
#include <coffi/coffi_headers.hpp>
#include <coffi/coffi_strings.hpp>
#include <coffi/coffi_symbols.hpp>
#include <coffi/coffi_directory.hpp>

#if defined(__has_include) && __has_include(<gsl/narrow>)
#include <gsl/narrow>
using gsl::narrow_cast;
#else
#ifndef narrow_cast
#define narrow_cast static_cast
#endif
#endif

//! COFFI library namespace
namespace COFFI {

//-------------------------------------------------------------------------
//! The COFFI library's main class.
class coffi : public coffi_strings,
              public coffi_symbols,
              public virtual sections_provider
{
  public:
    //---------------------------------------------------------------------
    coffi()
        : coffi_strings{}, architecture_{COFFI_ARCHITECTURE_NONE},
          dos_header_{0}, coff_header_{0}, optional_header_{0}, win_header_{0},
          directories_{this}
    {
        create(COFFI_ARCHITECTURE_PE);
    }

    //---------------------------------------------------------------------
    //! Discards the copy constructor
    coffi(const coffi&) = delete;

    //---------------------------------------------------------------------
    ~coffi() { clean(); }

    //---------------------------------------------------------------------
    /*! @brief Initializes the coffi object by loading data from COFF binary file.
         *
         * @param[in] file_name File path of the file to load.
         * @return true if the file was processed successfully, false otherwise.
         */
    bool load(const std::string& file_name)
    {
        std::ifstream stream;
        stream.open(file_name, std::ios::in | std::ios::binary);
        if (!stream) {
            return false;
        }

        return load(stream);
    }

    //---------------------------------------------------------------------
    /*! @brief @copybrief load(const std::string&)
         * 
         * See load(const std::string&).
         * 
         * @param[in] stream File to load, as an opened stream.
         */
    bool load(std::istream& stream)
    {
        clean();

        stream.seekg(0);

        architecture_ = COFFI_ARCHITECTURE_NONE;
        dos_header_   = new dos_header;
        if (dos_header_->load(stream)) {
            // It is an EXE
            architecture_ = COFFI_ARCHITECTURE_PE;
        }
        else {
            // It is not an EXE file, but it might be a PE OBJ file, or another type of COFF file
            clean();
            stream.seekg(0);
        }

        // Try to read a PE header
        coff_header_ = new coff_header_impl;
        if (coff_header_->load(stream)) {

            // Check the machine
            static const std::vector<uint16_t> machines = {
                IMAGE_FILE_MACHINE_AM33,      IMAGE_FILE_MACHINE_AMD64,
                IMAGE_FILE_MACHINE_ARM,       IMAGE_FILE_MACHINE_ARMNT,
                IMAGE_FILE_MACHINE_ARM64,     IMAGE_FILE_MACHINE_EBC,
                IMAGE_FILE_MACHINE_I386,      IMAGE_FILE_MACHINE_IA64,
                IMAGE_FILE_MACHINE_M32R,      IMAGE_FILE_MACHINE_MIPS16,
                IMAGE_FILE_MACHINE_MIPSFPU,   IMAGE_FILE_MACHINE_MIPSFPU16,
                IMAGE_FILE_MACHINE_POWERPC,   IMAGE_FILE_MACHINE_POWERPCFP,
                IMAGE_FILE_MACHINE_R4000,     IMAGE_FILE_MACHINE_SH3,
                IMAGE_FILE_MACHINE_SH3DSP,    IMAGE_FILE_MACHINE_SH4,
                IMAGE_FILE_MACHINE_SH5,       IMAGE_FILE_MACHINE_THUMB,
                IMAGE_FILE_MACHINE_WCEMIPSV2,
            };
            if (std::find(machines.begin(), machines.end(),
                          coff_header_->get_machine()) == machines.end()) {
                if (architecture_ == COFFI_ARCHITECTURE_PE) {
                    // The DOS header was detected, but the machine is not recognized
                    // This is an error
                    clean();
                    return false;
                }
            }
            else {
                // The machine is recognized, it is a PE file
                architecture_ = COFFI_ARCHITECTURE_PE;
            }

            // Try to read a CEVA header
            if (architecture_ == COFFI_ARCHITECTURE_NONE) {

                // Check the target ID
                static const std::vector<uint16_t> machines = {
                    CEVA_MACHINE_XC4210_LIB,
                    CEVA_MACHINE_XC4210_OBJ,
                };
                if (std::find(machines.begin(), machines.end(),
                              coff_header_->get_machine()) != machines.end()) {
                    architecture_ = COFFI_ARCHITECTURE_CEVA;
                }
            }
        }

        // Try to read a TI header
        if (architecture_ == COFFI_ARCHITECTURE_NONE) {

            coff_header_ = new coff_header_impl_ti;
            stream.seekg(0);
            if (!coff_header_->load(stream)) {
                clean();
                return false;
            }

            // Check the target ID
            static const std::vector<uint16_t> target_ids = {
                TMS470,      TMS320C5400, TMS320C6000,     TMS320C5500,
                TMS320C2800, MSP430,      TMS320C5500plus,
            };
            if (std::find(target_ids.begin(), target_ids.end(),
                          coff_header_->get_target_id()) != target_ids.end()) {
                architecture_ = COFFI_ARCHITECTURE_TI;
            }
        }

        if (architecture_ == COFFI_ARCHITECTURE_NONE) {
            // The format is not recognized, default to PE format header
            coff_header_ = new coff_header_impl;
            stream.seekg(0);
            if (!coff_header_->load(stream)) {
                clean();
                return false;
            }
        }

        if (coff_header_->get_optional_header_size()) {
            if (architecture_ == COFFI_ARCHITECTURE_PE) {
                std::streampos pos = stream.tellg();
                optional_header_   = new optional_header_impl_pe;
                if (!optional_header_->load(stream)) {
                    clean();
                    return false;
                }
                if (optional_header_->get_magic() == OH_MAGIC_PE32PLUS) {
                    delete optional_header_;
                    stream.seekg(pos);
                    optional_header_ = new optional_header_impl_pe_plus;
                    if (!optional_header_->load(stream)) {
                        clean();
                        return false;
                    }
                }
            }
            else {
                if (architecture_ == COFFI_ARCHITECTURE_NONE) {
                    optional_header_ = new optional_header_impl_pe;
                }
                if (architecture_ == COFFI_ARCHITECTURE_CEVA) {
                    optional_header_ = new optional_header_impl_pe;
                }
                if (architecture_ == COFFI_ARCHITECTURE_TI) {
                    optional_header_ = new optional_header_impl_ti;
                }
                if (!optional_header_->load(stream)) {
                    clean();
                    return false;
                }
            }

            if (architecture_ == COFFI_ARCHITECTURE_PE) {
                if (optional_header_->get_magic() == OH_MAGIC_PE32PLUS) {
                    win_header_ = new win_header_impl<win_header_pe_plus>;
                }
                else if (optional_header_->get_magic() == OH_MAGIC_PE32 ||
                         optional_header_->get_magic() == OH_MAGIC_PE32ROM) {
                    win_header_ = new win_header_impl<win_header_pe>;
                }

                if (win_header_) {
                    if (!win_header_->load(stream)) {
                        delete optional_header_;
                        optional_header_ = 0;
                        delete win_header_;
                        win_header_ = 0;
                        clean();
                        return false;
                    }
                    directories_.load(stream);
                }
            }

            if (architecture_ == COFFI_ARCHITECTURE_TI) {
                // No documented optional_header_->get_magic() values
                // Cannot check anything
            }
        }

        std::streampos pos = stream.tellg();
        if (!load_strings(stream, coff_header_)) {
            clean();
            return false;
        }

        if (!load_symbols(stream, coff_header_)) {
            clean();
            return false;
        }

        stream.seekg(pos);
        if (!load_section_headers(stream)) {
            clean();
            return false;
        }

        if (!directories_.load_data(stream)) {
            clean();
            return false;
        }

        return true;
    }

    //---------------------------------------------------------------------
    /*! @brief Creates a file in COFF binary format.
         *
         * Before saving, performs the following modififications:
         *   - Layout (see layout()): Compute the alignment, offsets, etc.
         *   - Compute the headers fields than can be guessed from the data, like:
         *       - Number of sections, directories, etc.
         *       - Sizes of headers
         *
         * @param[in] file_name File path of the file to create.
         * @return true if the file has been created successfully, false otherwise.
         */
    bool save(const std::string& file_name)
    {
        std::ofstream stream;
        stream.open(file_name, std::ios::out | std::ios::binary);
        if (!stream) {
            return false;
        }

        return save(stream);
    }

    //---------------------------------------------------------------------
    /*! @brief @copybrief save(const std::string&)
         *
         * See save(const std::string&).
         * 
         * @param[in] stream File to create, as an opened stream.
         */
    bool save(std::ostream& stream)
    {
        if (win_header_) {
            std::stringstream tmp_stream(std::ios::in | std::ios::out |
                                         std::ios::binary);
            if (!tmp_stream) {
                return false;
            }
            if (!save_to_stream(tmp_stream)) {
                return false;
            }
            tmp_stream.seekg(0);
            return compute_win_header_checksum(tmp_stream, stream);
        }
        else {
            return save_to_stream(stream);
        }
        return true;
    }

    //---------------------------------------------------------------------
    /*! @brief Cleans and/or initializes the coffi object.
         *
         * @param[in] architecture COFF architecture, see #coffi_architecture_t for the list of supported architectures
         */
    void create(coffi_architecture_t architecture)
    {
        clean();

        architecture_ = architecture;

        if (architecture_ == COFFI_ARCHITECTURE_PE) {
            coff_header_ = new coff_header_impl;
            coff_header_->set_machine(IMAGE_FILE_MACHINE_I386);
        }

        if (architecture_ == COFFI_ARCHITECTURE_CEVA) {
            coff_header_ = new coff_header_impl;
            coff_header_->set_machine(CEVA_MACHINE_XC4210_OBJ);
        }

        if (architecture_ == COFFI_ARCHITECTURE_TI) {
            coff_header_ = new coff_header_impl_ti;
            coff_header_->set_target_id(TMS320C2800);
        }
    }

    //---------------------------------------------------------------------
    /*! @brief Initializes an optional header for the coffi object.
         *
         * The optional header format depends on the architecture:
         *   - For PE files (#COFFI_ARCHITECTURE_PE), The following headers are created:
         *       - MS-DOS header: msdos_header
         *       - COFF optional header: coff_optional_header_pe or optional_header_impl_pe_plus (depending on **magic**)
         *       - Windows NT header: win_header_pe or win_header_pe_plus (depending on **magic**)
         *   - For TI files (#COFFI_ARCHITECTURE_TI): see common_optional_header_ti.
         *   - For CEVA files (#COFFI_ARCHITECTURE_CEVA): see coff_optional_header_pe.
         *
         * @param[in] magic Used only for the PE files (#COFFI_ARCHITECTURE_PE):
         * #OH_MAGIC_PE32 for PE32 format, #OH_MAGIC_PE32PLUS for PE32+ format.
         */
    void create_optional_header(uint16_t magic = OH_MAGIC_PE32)
    {
        if (dos_header_) {
            delete dos_header_;
        }
        if (optional_header_) {
            delete optional_header_;
        }
        if (architecture_ == COFFI_ARCHITECTURE_PE) {
            dos_header_ = new dos_header;
            if (magic == OH_MAGIC_PE32PLUS) {
                optional_header_ = new optional_header_impl_pe_plus;
            }
            else {
                optional_header_ = new optional_header_impl_pe;
            }
            optional_header_->set_magic(magic);
            create_win_header();
        }
        if (architecture_ == COFFI_ARCHITECTURE_CEVA) {
            optional_header_ = new optional_header_impl_pe;
        }
        if (architecture_ == COFFI_ARCHITECTURE_TI) {
            optional_header_ = new optional_header_impl_ti;
        }
        coff_header_->set_optional_header_size(narrow_cast<uint16_t>(
            optional_header_->get_sizeof() + win_header_->get_sizeof() +
            directories_.size() * sizeof(image_data_directory)));
    }

    //---------------------------------------------------------------------
    /*! @brief Returns the MS-DOS header
         *
         * @return nullptr if the MS-DOS header is not initialized (see create_optional_header()), or not loaded (see load()), or not relevant for the architecture.
         */
    dos_header* get_msdos_header() { return dos_header_; }

    //---------------------------------------------------------------------
    /*! @copydoc get_msdos_header()
         */
    const dos_header* get_msdos_header() const { return dos_header_; }

    //---------------------------------------------------------------------
    /*! @brief Returns the COFF header
         *
         * @return nullptr if the coffi object is not initialized (see create()), or not loaded (see load()).
         */
    coff_header* get_header() { return coff_header_; }

    //---------------------------------------------------------------------
    /*! @copydoc get_header()
         */
    const coff_header* get_header() const { return coff_header_; }

    //---------------------------------------------------------------------
    /*! @brief Returns the optional COFF header
         *
         * @return nullptr if the optional COFF header is not initialized (see create_optional_header()), or not loaded (see load()).
         */
    optional_header* get_optional_header() { return optional_header_; }

    //---------------------------------------------------------------------
    /*! @copydoc get_optional_header()
         */
    const optional_header* get_optional_header() const
    {
        return optional_header_;
    }

    //---------------------------------------------------------------------
    /*! @brief Returns the Windows NT header
         *
         * @return nullptr if the Windows NT header is not initialized (see create_optional_header()), or not loaded (see load()), or not relevant for the architecture.
         */
    win_header* get_win_header() { return win_header_; }

    //---------------------------------------------------------------------
    /*! @copydoc get_win_header()
         */
    const win_header* get_win_header() const { return win_header_; }

    //---------------------------------------------------------------------
    /*! @brief Returns a list of the COFF sections
         *
         * @return Empty list if the coffi object is not initialized.
         */
    sections& get_sections() { return sections_; }

    //---------------------------------------------------------------------
    /*! @copydoc get_sections()
         */
    const sections& get_sections() const { return sections_; }

    //---------------------------------------------------------------------
    /*! @brief Add a COFF section
         *
         * @param[in] name The section name
         * @return A pointer to the newly created section.
         */
    section* add_section(const std::string& name)
    {
        section* sec;
        if (architecture_ == COFFI_ARCHITECTURE_TI) {
            sec = new section_impl_ti{this, this, this};
        }
        else {
            sec = new section_impl{this, this, this};
        }
        sec->set_index(narrow_cast<uint32_t>(sections_.size()));
        sec->set_name(name);
        sections_.push_back(sec);
        return sec;
    }

    //---------------------------------------------------------------------
    /*! @brief Returns a list of the PE data directories.
         *
         * This function is relevant only for the PE architecture (see #COFFI_ARCHITECTURE_PE).
         *
         * @return Empty list if the PE data directories are not initialized, or not relevant for the architecture.
         */
    directories& get_directories() { return directories_; }

    //---------------------------------------------------------------------
    /*! @copydoc get_directories()
         */
    const directories& get_directories() const { return directories_; }

    //---------------------------------------------------------------------
    /*! @brief Add a PE data directory.
         *
         * This function is relevant only for the PE architecture (see #COFFI_ARCHITECTURE_PE).
         *
         * @param[in] rva_and_size Relative virtual address (RVA) and size
         * @return A pointer to the newly created directory.
         */
    directory* add_directory(const image_data_directory& rva_and_size)
    {
        directory* d =
            new directory(narrow_cast<uint32_t>(directories_.size()));
        d->set_virtual_address(rva_and_size.virtual_address);
        d->set_size(rva_and_size.size);
        directories_.push_back(d);
        return d;
    }

    //---------------------------------------------------------------------
    /*! @brief PE32+ format check.
         *
         * @return true if the file is initialized and is a PE file (see #COFFI_ARCHITECTURE_PE), with a magic number indicating a PE32+ file (see #OH_MAGIC_PE32PLUS).
         */
    bool is_PE32_plus()
    {
        bool ret = false;
        if (optional_header_ &&
            optional_header_->get_magic() == OH_MAGIC_PE32PLUS) {
            ret = true;
        }
        return ret;
    }

    //---------------------------------------------------------------------
    //! @copydoc architecture_provider::get_addressable_unit()
    int get_addressable_unit() const
    {
        if (!coff_header_) {
            return 0;
        }
        if (architecture_ == COFFI_ARCHITECTURE_TI) {
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
    //! @copydoc architecture_provider::get_architecture()
    coffi_architecture_t get_architecture() const { return architecture_; }

    //---------------------------------------------------------------------
    /*! @brief Performs the layout of the file.
         *
         * The layout consists in:
         *   - Compute the sections alignment,
         *   - Compute the offsets for: sections, directories, relocations, line numbers, string table offset, etc.
         */
    void layout()
    {
        // Order all the data pages to be written
        clean_unused_spaces();
        populate_data_pages();
        compute_offsets();

        // Eventually add unused spaces to respect the file alignment
        populate_data_pages();
        apply_file_alignment();
        populate_data_pages();
        compute_offsets();
    }

    //---------------------------------------------------------------------
  private:
    //---------------------------------------------------------------------
    void clean()
    {
        if (dos_header_) {
            delete dos_header_;
            dos_header_ = 0;
        }

        if (coff_header_) {
            delete coff_header_;
            coff_header_ = 0;
        }

        if (optional_header_) {
            delete optional_header_;
            optional_header_ = 0;
        }

        if (win_header_) {
            delete win_header_;
            win_header_ = 0;
        }

        clean_unused_spaces();

        sections_.clean();

        directories_.clean();

        clean_symbols();
        clean_strings();
    }

    //---------------------------------------------------------------------
    void create_win_header()
    {
        if (win_header_) {
            delete win_header_;
        }
        if (optional_header_->get_magic() == OH_MAGIC_PE32PLUS) {
            win_header_ = new win_header_impl<win_header_pe_plus>;
        }
        else if (optional_header_->get_magic() == OH_MAGIC_PE32 ||
                 optional_header_->get_magic() == OH_MAGIC_PE32ROM) {
            win_header_ = new win_header_impl<win_header_pe>;
        }
    }

    //---------------------------------------------------------------------
    bool load_section_headers(std::istream& stream)
    {
        std::streampos pos = stream.tellg();
        for (int i = 0; i < coff_header_->get_sections_count(); ++i) {
            section* sec;
            switch (architecture_) {
            case COFFI_ARCHITECTURE_PE:
            case COFFI_ARCHITECTURE_CEVA:
                sec = new section_impl(this, this, this);
                break;
            case COFFI_ARCHITECTURE_TI:
                sec = new section_impl_ti(this, this, this);
                break;
            default:
                sec = new section_impl(this, this, this);
                break;
            }
            if (!(sec->load(stream, i * sec->get_sizeof() + pos))) {
                return false;
            }
            sec->set_index(i);
            sections_.push_back(sec);
        }

        return true;
    }

    //---------------------------------------------------------------------
    bool save_to_stream(std::ostream& stream)
    {
        // Layout the sections and other data pages
        layout();

        // Compute the header fields
        coff_header_->set_sections_count(
            narrow_cast<uint16_t>(sections_.size()));
        if (symbols_.size() > 0) {
            coff_header_->set_symbols_count(
                symbols_.back().get_index() +
                symbols_.back().get_aux_symbols_number() + 1);
        }
        else {
            coff_header_->set_symbols_count(0);
        }
        coff_header_->set_optional_header_size(0);
        if (optional_header_) {
            coff_header_->set_optional_header_size(
                coff_header_->get_optional_header_size() +
                narrow_cast<uint16_t>(optional_header_->get_sizeof()));
        }
        if (win_header_) {
            win_header_->set_number_of_rva_and_sizes(
                narrow_cast<uint32_t>(directories_.size()));
            coff_header_->set_optional_header_size(narrow_cast<uint16_t>(
                coff_header_->get_optional_header_size() +
                win_header_->get_sizeof() + directories_.get_sizeof()));
        }

        if ((architecture_ == COFFI_ARCHITECTURE_PE) && dos_header_) {
            dos_header_->save(stream);
        }

        coff_header_->save(stream);

        if (optional_header_) {
            optional_header_->save(stream);

            if (architecture_ == COFFI_ARCHITECTURE_PE) {

                if (win_header_) {
                    win_header_->save(stream);
                    directories_.save(stream);
                }
            }
        }

        for (auto sec : sections_) {
            sec->save_header(stream);
        }

        for (auto dp : data_pages_) {
            section* sec;
            switch (dp.type) {
            case DATA_PAGE_RAW:
                sec = sections_[dp.index];
                sec->save_data(stream);
                break;
            case DATA_PAGE_RELOCATIONS:
                sec = sections_[dp.index];
                sec->save_relocations(stream);
                break;
            case DATA_PAGE_LINE_NUMBERS:
                sec = sections_[dp.index];
                sec->save_line_numbers(stream);
                break;
            case DATA_PAGE_DIRECTORY:
                directories_[dp.index]->save_data(stream);
                break;
            case DATA_PAGE_UNUSED:
                stream.write(unused_spaces_[dp.index].data,
                             unused_spaces_[dp.index].size);
                break;
            }
        }

        save_symbols(stream);

        save_strings(stream);

        return true;
    }

    //---------------------------------------------------------------------
    bool compute_win_header_checksum(std::istream& src, std::ostream& dst)
    {
        if (!dos_header_ || !coff_header_ || !optional_header_ ||
            !win_header_) {
            return false;
        }

        // Get the file size
        src.seekg(0, std::ios::end);
        uint32_t file_size = narrow_cast<uint32_t>(src.tellg());
        src.seekg(0);

        // Compute the checksum offset
        uint32_t chk_offset =
            dos_header_->get_pe_sign_location() + 4 +
            narrow_cast<uint32_t>(coff_header_->get_sizeof()) +
            narrow_cast<uint32_t>(optional_header_->get_sizeof()) +
            (is_PE32_plus() ? 40 : 36);

        // Copy the file and compute the checksum
        uint32_t chk = 0;
        for (uint32_t i = 0; i < file_size; i += 2) {
            char bytes[2] = {0, 0};
            src.read(bytes, 2);
            dst.write(bytes, src.gcount());
            uint16_t word = *(reinterpret_cast<uint16_t*>(bytes));
            if ((i >= chk_offset) && (i < chk_offset + 4)) {
                word = 0;
            }
            chk += word;
            chk = (chk >> 16) + (chk & 0xffff);
        }
        chk = (uint16_t)(((chk >> 16) + chk) & 0xffff);
        chk += file_size;

        // Update the checksum in the dst stream
        dst.seekp(chk_offset);
        dst.write(reinterpret_cast<char*>(&chk), 4);
        return true;
    }

    //---------------------------------------------------------------------
    void populate_data_pages()
    {
        data_pages_.clear();
        for (auto sec : sections_) {
            if (sec->get_data_offset() > 0 || sec->get_data()) {
                data_pages_.push_back(
                    data_page{DATA_PAGE_RAW, sec->get_data_offset(),
                              sec->get_data_size(), sec->get_index()});
            }
            if (sec->get_reloc_offset() > 0 || sec->get_reloc_count() > 0) {
                data_pages_.push_back(data_page{
                    DATA_PAGE_RELOCATIONS, sec->get_reloc_offset(),
                    sec->get_relocations_filesize(), sec->get_index()});
            }
            if ((architecture_ != COFFI_ARCHITECTURE_TI) &&
                (sec->get_line_num_count() > 0)) {
                data_pages_.push_back(data_page{
                    DATA_PAGE_LINE_NUMBERS, sec->get_line_num_offset(),
                    sec->get_line_numbers_filesize(), sec->get_index()});
            }
        }
        for (auto d : directories_) {
            if (d->get_data_filesize() > 0) {
                data_pages_.push_back(data_page{DATA_PAGE_DIRECTORY,
                                                d->get_virtual_address(),
                                                d->get_size(), d->get_index()});
            }
        }

        for (uint32_t i = 0; i < unused_spaces_.size(); i++) {
            data_pages_.push_back(data_page{DATA_PAGE_UNUSED,
                                            unused_spaces_[i].offset,
                                            unused_spaces_[i].size, i});
        }

        std::sort(data_pages_.begin(), data_pages_.end(),
                  [](const data_page& a, const data_page& b) {
                      if (a.offset != b.offset) {
                          return a.offset < b.offset;
                      }
                      if (a.type != b.type) {
                          return a.type < b.type;
                      }
                      return a.index < b.index;
                  });
    }

    //---------------------------------------------------------------------
    uint32_t get_header_end_offset()
    {
        uint32_t offset = 0;
        if (dos_header_) {
            offset = dos_header_->get_pe_sign_location();
            offset += 4;
        }
        if (coff_header_) {
            offset += narrow_cast<uint32_t>(coff_header_->get_sizeof());
        }
        if (optional_header_) {
            offset += narrow_cast<uint32_t> (optional_header_->get_sizeof());
        }
        if (win_header_) {
            offset += narrow_cast<uint32_t> (win_header_->get_sizeof());
        }
        offset += directories_.get_sizeof();
        for (auto sec : sections_) {
            offset += narrow_cast<uint32_t> (sec->get_sizeof());
        }
        return offset;
    }

    //---------------------------------------------------------------------
    void compute_offsets()
    {
        uint32_t offset = get_header_end_offset();

        for (auto dp : data_pages_) {
            auto sec = sections_[dp.index];
            switch (dp.type) {
            case DATA_PAGE_RAW:
                if (sec->get_data()) {
                    sec->set_data_offset(offset);
                    offset += sec->get_data_size();
                }
                else if (sec->get_data_offset() != 0) {
                    sec->set_data_offset(offset);
                }
                break;
            case DATA_PAGE_RELOCATIONS:
                if (sec->get_reloc_count() > 0) {
                    sec->set_reloc_offset(offset);
                }
                else {
                    sec->set_reloc_offset(0);
                }
                offset += sec->get_relocations_filesize();
                break;
            case DATA_PAGE_LINE_NUMBERS:
                if (sec->get_line_num_count() > 0) {
                    sec->set_line_num_offset(offset);
                }
                else {
                    sec->set_line_num_offset(0);
                }
                offset += sec->get_line_numbers_filesize();
                break;
            case DATA_PAGE_DIRECTORY:
                if (directories_[dp.index]->get_data_filesize() > 0) {
                    if (directories_[dp.index]->get_virtual_address() != 0) {
                        directories_[dp.index]->set_virtual_address(offset);
                    }
                    offset += directories_[dp.index]->get_data_filesize();
                }
                break;
            case DATA_PAGE_UNUSED:
                offset += unused_spaces_[dp.index].size;
                break;
            }
        }

        if (symbols_.size() == 0) {
            offset = 0;
        }
        coff_header_->set_symbol_table_offset(offset);
    }

    //---------------------------------------------------------------------
    void apply_file_alignment()
    {
        if (!win_header_) {
            return;
        }
        uint32_t         file_alignment  = win_header_->get_file_alignment();
        uint32_t         previous_offset = get_header_end_offset();
        uint32_t         previous_size   = previous_offset;
        const data_page* previous_dp     = nullptr;
        for (auto dp = data_pages_.begin(); dp != data_pages_.end(); dp++) {
            if ((previous_size % file_alignment) != 0) {
                uint32_t size =
                    file_alignment - (previous_size % file_alignment);
                if (previous_dp && previous_dp->type == DATA_PAGE_RAW) {
                    // Extend the previous section data
                    char* padding = new char[size];
                    if (padding) {
                        std::memset(padding, 0, size);
                        sections_[previous_dp->index]->append_data(padding,
                                                                   size);
                    }
                }
                else {
                    // Add an unused space
                    add_unused_space(previous_offset, size);
                }
            }
            previous_dp   = &*dp;
            previous_size = dp->size;
            if (dp->offset > 0) {
                previous_offset = dp->offset + dp->size;
            }
        }
    }

    //---------------------------------------------------------------------
    void clean_unused_spaces()
    {
        for (auto us : unused_spaces_) {
            delete[] us.data;
        }
        unused_spaces_.clear();
    }

    //---------------------------------------------------------------------
    void
    add_unused_space(uint32_t offset, uint32_t size, uint8_t padding_byte = 0)
    {
        unused_space us;
        us.data = new char[size];
        if (us.data) {
            std::memset(us.data, padding_byte, size);
            us.size   = size;
            us.offset = offset;
            unused_spaces_.push_back(us);
        }
    }

    //---------------------------------------------------------------------
    enum data_page_type
    {
        DATA_PAGE_UNUSED,
        DATA_PAGE_RAW,
        DATA_PAGE_RELOCATIONS,
        DATA_PAGE_LINE_NUMBERS,
        DATA_PAGE_DIRECTORY,
    };

    //---------------------------------------------------------------------
    struct data_page
    {
        data_page_type type;
        uint32_t       offset;
        uint32_t       size;
        // Section index, directory index, index in unused_spaces_
        uint32_t index;
    };

    //---------------------------------------------------------------------
    struct unused_space
    {
        uint32_t offset;
        uint32_t size;
        char*    data;
    };

    //---------------------------------------------------------------------
    coffi_architecture_t      architecture_;
    dos_header*               dos_header_;
    coff_header*              coff_header_;
    optional_header*          optional_header_;
    win_header*               win_header_;
    directories               directories_;
    sections                  sections_;
    std::vector<unused_space> unused_spaces_;
    std::vector<data_page>    data_pages_;
};

} // namespace COFFI

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //COFFI_HPP
