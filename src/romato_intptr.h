////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the romato library.
//
// Copyright 2013-2022 Rocco Matano
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

#ifdef  __cplusplus

////////////////////////////////////////////////////////////////////////////////

// convert any pointer type to another pointer type
template <class P> P p2p(void* ptr)
{
    static_assert(std::is_pointer<P>::value, "P must be a pointer type");
    return reinterpret_cast<P>(ptr);
}

template <class P> P p2p(const void* ptr)
{
    static_assert(std::is_pointer<P>::value, "P must be a pointer type");
    return reinterpret_cast<P>(ptr);
}

////////////////////////////////////////////////////////////////////////////////

template <class P> P typed_null()
{
    static_assert(std::is_pointer<P>::value, "P must be a pointer type");
    return static_cast<P>(nullptr);
}

////////////////////////////////////////////////////////////////////////////////

// convert any pointer type to a wide enough integer type
template <class I> I p2i(void* ptr)
{
    static_assert(std::is_integral<I>::value, "I must be an integral type");
    static_assert(sizeof(I) >= sizeof(void*), "I is not wide enough");
    return reinterpret_cast<I>(ptr);
}

template <class I> I p2i(const void* ptr)
{
    static_assert(std::is_integral<I>::value, "I must be an integral type");
    static_assert(sizeof(I) >= sizeof(void*), "I is not wide enough");
    return reinterpret_cast<I>(ptr);
}

////////////////////////////////////////////////////////////////////////////////

// passing pointers via the LPARAM of SendMessage is so common, that it is
// worth of a explicit specialization.
static_assert(sizeof(LPARAM) >= sizeof(void*), "insane: LPARAM < void*");

inline LPARAM p2lp(void* ptr)
{
    return reinterpret_cast<LPARAM>(ptr);
}

inline LPARAM p2lp(const void* ptr)
{
    return reinterpret_cast<LPARAM>(ptr);
}

////////////////////////////////////////////////////////////////////////////////

// convert any integer type to a wide enough pointer type
template <class P, class I> P i2p(I i)
{
    static_assert(std::is_pointer<P>::value, "P must be a pointer type");
    static_assert(std::is_integral<I>::value, "I must be an integral type");
    static_assert(sizeof(P) >= sizeof(I), "P is not wide enough");
    return reinterpret_cast<P>(i);
}

////////////////////////////////////////////////////////////////////////////////

// round up ptr, so that is aligned for an element that P points to
template <class P> P align_ptr(void* ptr)
{
    static_assert(std::is_pointer<P>::value, "P must be a pointer type");
    typedef typename std::remove_pointer<P>::type noptr;
    const uintptr_t aligned = align_to_pwr2(
        reinterpret_cast<uintptr_t>(ptr),
        alignof(noptr)
        );
    return reinterpret_cast<P>(aligned);
}

template <class P> P align_ptr(const void* ptr)
{
    static_assert(std::is_pointer<P>::value, "P must be a pointer type");
    typedef typename std::remove_pointer<P>::type noptr;
    const uintptr_t aligned = align_to_pwr2(
        reinterpret_cast<uintptr_t>(ptr),
        alignof(noptr)
        );
    return reinterpret_cast<P>(aligned);
}

#endif //  __cplusplus
