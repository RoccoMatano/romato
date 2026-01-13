////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the romato library.
//
// Copyright 2013-2026 Rocco Matano
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "romato_macros.h"


#ifdef _MSC_VER
#pragma intrinsic(memset, memcpy, memcmp)
#if _MSC_VER < 1600
#ifndef _CRTNOALIAS
#define _CRTNOALIAS __declspec(noalias)
#endif
#endif
#endif // _MSC_VER

#ifndef _CRTNOALIAS
#define _CRTNOALIAS
#endif

#ifdef  __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////

_CRTNOALIAS _CRTRESTRICT void* __cdecl malloc(size_t size);
_CRTNOALIAS void __cdecl free(void* p);

//////////////////////////////////////////////////////////////////////////////

void* aligned_malloc(size_t size, size_t align);
void aligned_free(void *aligned_mem);

//////////////////////////////////////////////////////////////////////////////

void* __cdecl memset(void* dst, int val, size_t count);
void* __cdecl memcpy(void* dst, const void* src, size_t count);
void* __cdecl memmove(void* dst, const void* src, size_t count);
int   __cdecl memcmp(void const* p1, void const* p2, size_t count);

//////////////////////////////////////////////////////////////////////////////

#ifdef  __cplusplus
} // extern "C"

#include <type_traits>
#include <memory>

class any_buffer
{
    uint8_t *m_ptr;

public:

    any_buffer(size_t size) : m_ptr(new uint8_t[size])
    {
    }
    any_buffer(const any_buffer&) = delete;
    any_buffer& operator=(const any_buffer& src) = delete;

    ~any_buffer()
    {
        delete[] m_ptr;
    }

    template <typename T> T ptr()
    {
        static_assert(std::is_pointer<T>::value, "T must be a pointer type");
        return reinterpret_cast<T>(m_ptr);
    }
};

template<typename T> class auto_buffer
{
    uint8_t *m_ptr;

public:

    auto_buffer(size_t size) : m_ptr(new uint8_t[size])
    {
    }
    auto_buffer(const auto_buffer&) = delete;
    auto_buffer& operator=(const auto_buffer& src) = delete;

    ~auto_buffer()
    {
        delete[] m_ptr;
    }

    T operator&()
    {
        static_assert(std::is_pointer<T>::value, "T must be a pointer type");
        return reinterpret_cast<T>(m_ptr);
    }
};

template <class T> std::shared_ptr<T> shared_buffer(size_t size)
{
    return std::shared_ptr<T>(reinterpret_cast<T*>(new uint8_t[size]));
}

#endif // __cplusplus
