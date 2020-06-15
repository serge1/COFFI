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

#ifndef COFFI_UTILS_HPP
#define COFFI_UTILS_HPP

#include <stdexcept>
#include <coffi/coffi_types.hpp>

#define STRINGIFY(NAME) #NAME

#define COFFI_GET_ACCESS_DECL( TYPE, NAME )       \
    virtual TYPE get_##NAME() const = 0

#define COFFI_SET_ACCESS_DECL( TYPE, NAME )       \
    virtual void set_##NAME( TYPE value ) = 0

#define COFFI_GET_SET_ACCESS_DECL( TYPE, NAME )   \
    virtual TYPE get_##NAME() const = 0;          \
    virtual void set_##NAME( TYPE value ) = 0

#define COFFI_GET_ACCESS( TYPE, NAME )      \
    TYPE get_##NAME() const                 \
    {                                       \
        return header.NAME;                 \
    }

#define COFFI_SET_ACCESS( TYPE, NAME )      \
    void set_##NAME( TYPE value )           \
    {                                       \
        header.NAME = value;                \
    }

#define COFFI_GET_SET_ACCESS( TYPE, NAME )  \
    TYPE get_##NAME() const                 \
    {                                       \
        return header.NAME;                 \
    }                                       \
    void set_##NAME( TYPE value )           \
    {                                       \
        header.NAME = value;                \
    }

#define COFFI_GET_ACCESS_NONE( TYPE, NAME )               \
    TYPE get_##NAME() const                               \
    {                                                     \
        throw std::runtime_error("The header field '"     \
            STRINGIFY(NAME)                               \
            "' is not applicable to this COFF version");  \
    }

#define COFFI_SET_ACCESS_NONE( TYPE, NAME )               \
    void set_##NAME( TYPE value )                         \
    {                                                     \
        throw std::runtime_error("The header field '"     \
            STRINGIFY(NAME)                               \
            "' is not applicable to this COFF version");  \
    }

#define COFFI_GET_SET_ACCESS_NONE( TYPE, NAME )           \
    TYPE get_##NAME() const                               \
    {                                                     \
        throw std::runtime_error("The header field '"     \
            STRINGIFY(NAME)                               \
            "' is not applicable to this COFF version");  \
    }                                                     \
    void set_##NAME( TYPE )                               \
    {                                                     \
        throw std::runtime_error("The header field '"     \
            STRINGIFY(NAME)                               \
            "' is not applicable to this COFF version");  \
    }

#define COFFI_GET_SIZEOF_DECL()             \
    virtual size_t get_sizeof() const = 0

#define COFFI_GET_SIZEOF()                  \
    size_t get_sizeof() const               \
    {                                       \
        return sizeof(header);              \
    }

#endif // COFFI_UTILS_HPP
