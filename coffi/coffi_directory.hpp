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

/*! @file coffi_directory.hpp 
 * @brief COFFI library classes for the PE data directories.
 *
 * Do not include this file directly. This file is included by coffi.hpp.
 */

#ifndef COFFI_DIRECTORY_HPP
#define COFFI_DIRECTORY_HPP

#include <iostream>
#include <vector>

#include <coffi/coffi_utils.hpp>
#include <coffi/coffi_headers.hpp>
#include <coffi/coffi_section.hpp>

#if defined(__has_include) && __has_include(<gsl/narrow>)
#include <gsl/narrow>
using gsl::narrow_cast;
#else
#ifndef narrow_cast
#define narrow_cast static_cast
#endif
#endif

namespace COFFI {

//! Class for accessing an image data directory
class directory
{
  public:
    //------------------------------------------------------------------------------
    directory(uint32_t index) : data_{nullptr}, index_{index} {}

    //------------------------------------------------------------------------------
    //! Discards the copy constructor
    directory(const directory&) = delete;

    virtual ~directory() { clean(); }
    //------------------------------------------------------------------------------
    //! @accessors{directory}
    COFFI_GET_SET_ACCESS(uint32_t, virtual_address);
    COFFI_GET_SET_ACCESS(uint32_t, size);

    COFFI_GET_SIZEOF();
    //! @endaccessors

    uint32_t get_index() const { return index_; }

    //------------------------------------------------------------------------------
    const char* get_data() const { return data_; }

    //------------------------------------------------------------------------------
    void set_data(const char* data, uint32_t size)
    {
        if ((index_ != DIRECTORY_CERTIFICATE_TABLE) &&
            (index_ != DIRECTORY_BOUND_IMPORT)) {
            return;
        }

        clean();
        if (!data) {
            set_size(0);
            return;
        }

        char* temp_buffer = new char[size];
        if (!temp_buffer) {
            set_size(0);
            return;
        }
        std::copy(data, data + size, temp_buffer);
        data_ = temp_buffer;
        set_size(size);
    }

    //------------------------------------------------------------------------------
    uint32_t get_data_filesize() const
    {
        if ((index_ == DIRECTORY_CERTIFICATE_TABLE) ||
            (index_ == DIRECTORY_BOUND_IMPORT)) {
            return get_size();
        }
        return 0;
    }

    //------------------------------------------------------------------------------
    bool load(std::istream& stream)
    {
        clean();
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (stream.gcount() != sizeof(header)) {
            return false;
        }
        return true;
    }

    //------------------------------------------------------------------------------
    bool load_data(std::istream& stream)
    {
        if ((index_ != DIRECTORY_CERTIFICATE_TABLE) &&
            (index_ != DIRECTORY_BOUND_IMPORT)) {
            return true;
        }
        if ((get_size() > 0) && (get_virtual_address() != 0)) {
            char* temp_buffer = new char[get_size()];
            stream.seekg(get_virtual_address());
            stream.read(temp_buffer, get_size());
            if (stream.gcount() != static_cast<int>(get_size())) {
                return false;
            }
            data_ = temp_buffer;
        }
        return true;
    }

    //---------------------------------------------------------------------
    void save(std::ostream& stream) const
    {
        stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    }

    void save_data(std::ostream& stream) const
    {
        if ((index_ != DIRECTORY_CERTIFICATE_TABLE) &&
            (index_ != DIRECTORY_BOUND_IMPORT)) {
            return;
        }
        if (data_ && get_size() > 0) {
            stream.write(data_, get_size());
        }
    }

    //------------------------------------------------------------------------------
    void clean()
    {
        if (data_) {
            delete[] data_;
            data_ = nullptr;
        }
    }

  private:
    image_data_directory header;
    const char*          data_;
    uint32_t             index_;
};

/*! @brief List of image data directories
     *
     * It is implemented as a vector of @ref directory pointers.
     */
class directories : public std::vector<directory*>
{
  public:
    //------------------------------------------------------------------------------
    directories(sections_provider* scn) : scn_{scn} {}

    //! Discards the copy constructor
    directories(const directories&) = delete;

    virtual ~directories() { clean(); }

    void clean()
    {
        for (auto d : *this) {
            delete d;
        }
        clear();
    }

    //------------------------------------------------------------------------------
    bool load(std::istream& stream)
    {
        for (uint32_t i = 0;
             i < scn_->get_win_header()->get_number_of_rva_and_sizes(); ++i) {
            directory* d = new directory(i);
            if (!d->load(stream)) {
                return false;
            }
            push_back(d);
        }
        return true;
    }

    //------------------------------------------------------------------------------
    bool load_data(std::istream& stream)
    {
        for (auto d : *this) {
            if (!d->load_data(stream)) {
                return false;
            }
        }
        return true;
    }

    //---------------------------------------------------------------------
    void save(std::ostream& stream) const
    {
        for (auto d : *this) {
            d->save(stream);
        }
    }

    //------------------------------------------------------------------------------
    uint32_t get_sizeof() const
    {
        if (size() > 0) {
            return narrow_cast<uint32_t>(size() * (*begin())->get_sizeof());
        }
        return 0;
    }

    //------------------------------------------------------------------------------
  protected:
    //------------------------------------------------------------------------------
    sections_provider* scn_;
};

} // namespace COFFI

#endif //COFFI_DIRECTORY_HPP
