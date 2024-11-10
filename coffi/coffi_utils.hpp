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

/*! @file coffi_utils.hpp
 * @brief COFFI library utilities.
 *
 * Do not include this file directly. This file is included by coffi.hpp.
 */

#ifndef COFFI_UTILS_HPP
#define COFFI_UTILS_HPP

#include <stdexcept>
#include <coffi/coffi_types.hpp>

#define STRINGIFY(NAME) #NAME

/*! @name Macros for accessing the COFF structures fields
 * @anchor group_access
 *
 * These macros provide a way to declare and define the get/set accessors to the COFF structures fields.
 * <br>The list of the classes using these macro, and the COFF structures accessed is given by: @ref accessors.
 *
 * The declaration functions <b>*_DECL</b> are used in virtual template classes.
 * <br>They declare the interface for all the classes, derived from the same virtual template class.
 * <br>This is useful when a COFF structure has several flavors, depending on the COFF file architecture.
 *
 * @{
 */

//! Declares a **get_NAME** function for accessing the **NAME** structure field.
#define COFFI_GET_ACCESS_DECL(TYPE, NAME) virtual TYPE get_##NAME() const = 0

//! Declares a **set_NAME** function for accessing the **NAME** structure field.
#define COFFI_SET_ACCESS_DECL(TYPE, NAME) \
    virtual void set_##NAME(TYPE value) = 0

//! Declares a **get_NAME** and a **set_NAME** functions for accessing the **NAME** structure field.
#define COFFI_GET_SET_ACCESS_DECL(TYPE, NAME) \
    virtual TYPE get_##NAME() const     = 0;  \
    virtual void set_##NAME(TYPE value) = 0

//! Defines a **get_NAME** function for accessing the **NAME** structure field.
#define COFFI_GET_ACCESS(TYPE, NAME) \
    TYPE get_##NAME() const { return header.NAME; }

//! Defines a **set_NAME** function for accessing the **NAME** structure field.
#define COFFI_SET_ACCESS(TYPE, NAME) \
    void set_##NAME(TYPE value) { header.NAME = value; }

//! Defines a **get_NAME** and a **set_NAME** functions for accessing the **NAME** structure field.
#define COFFI_GET_SET_ACCESS(TYPE, NAME)            \
    TYPE get_##NAME() const { return header.NAME; } \
    void set_##NAME(TYPE value) { header.NAME = value; }

//! Disables the **get_NAME** function for prohibiting read accesses to the **NAME** structure field.
#define COFFI_GET_ACCESS_NONE(TYPE, NAME)                        \
    TYPE get_##NAME() const                                      \
    {                                                            \
        throw std::runtime_error("The header field '" STRINGIFY( \
            NAME) "' is not applicable to this COFF version");   \
    }

//! Disables the **set_NAME** function for prohibiting write accesses to the **NAME** structure field.
#define COFFI_SET_ACCESS_NONE(TYPE, NAME)                        \
    void set_##NAME(TYPE value)                                  \
    {                                                            \
        throw std::runtime_error("The header field '" STRINGIFY( \
            NAME) "' is not applicable to this COFF version");   \
    }

//! Disables the **get_NAME** and the **set_NAME** function for prohibiting all accesses to the **NAME** structure field.
#define COFFI_GET_SET_ACCESS_NONE(TYPE, NAME)                    \
    TYPE get_##NAME() const                                      \
    {                                                            \
        throw std::runtime_error("The header field '" STRINGIFY( \
            NAME) "' is not applicable to this COFF version");   \
    }                                                            \
    void set_##NAME(TYPE)                                        \
    {                                                            \
        throw std::runtime_error("The header field '" STRINGIFY( \
            NAME) "' is not applicable to this COFF version");   \
    }

//! Declares the **get_sizeof** function for returning the size of the COFF file structure.
#define COFFI_GET_SIZEOF_DECL() virtual size_t get_sizeof() const = 0

//! Defines the **get_sizeof** function for returning the size of the COFF file structure.
#define COFFI_GET_SIZEOF() \
    size_t get_sizeof() const { return sizeof(header); }

//! @}

template <typename T> class unique_ptr_collection
{
  public:
    class iterator
    {
      public:
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = value_type*;
        using reference         = value_type&;

        explicit iterator(
            typename std::vector<std::unique_ptr<T>>::iterator&& iterator)
            : iterator_(std::move(iterator))
        {
        }

        reference operator*() const { return *iterator_->get(); }

        pointer operator->() { return iterator_->get(); }

        iterator& operator++()
        {
            ++iterator_;
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            iterator_++;
            return tmp;
        }

        friend bool operator==(const iterator& lhs, const iterator& rhs)
        {
            return lhs.iterator_ == rhs.iterator_;
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs)
        {
            return lhs.iterator_ != rhs.iterator_;
        }

      private:
        typename std::vector<std::unique_ptr<T>>::iterator iterator_;
    };

    class const_iterator
    {
      public:
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = const value_type*;
        using reference         = const value_type&;

        explicit const_iterator(
            typename std::vector<std::unique_ptr<T>>::const_iterator && iterator)
            : iterator_(std::move(iterator))
        {
        }

        reference operator*() const { return *iterator_->get(); }

        pointer operator->() { return iterator_->get(); }

        const_iterator& operator++()
        {
            ++iterator_;
            return *this;
        }

        const_iterator operator++(int)
        {
            iterator tmp = *this;
            iterator_++;
            return tmp;
        }

        friend bool operator==(const const_iterator& lhs, const const_iterator& rhs)
        {
            return lhs.iterator_ == rhs.iterator_;
        }

        friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs)
        {
            return lhs.iterator_ != rhs.iterator_;
        }

      private:
        typename std::vector<std::unique_ptr<T>>::const_iterator iterator_;
    };

    T* operator[](size_t i)
    {
        return items_[i].get();
    }

    const T* operator[](size_t i) const
    {
        return items_[i].get();
    }

    void clean() { items_.clear(); }

    [[nodiscard]] size_t get_count() const { return items_.size(); }

    void append(std::unique_ptr<T>&& item)
    {
        items_.emplace_back(std::move(item));
    }

    iterator begin() { return iterator(items_.begin()); }
    iterator end() { return iterator(items_.end()); }

    const_iterator begin() const { return const_iterator(items_.begin()); }
    const_iterator end() const { return const_iterator(items_.end()); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

  protected:
    std::vector<std::unique_ptr<T>> items_;
};

#endif // COFFI_UTILS_HPP
