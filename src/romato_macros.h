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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef _CRTRESTRICT
#define _CRTRESTRICT
#endif

////////////////////////////////////////////////////////////////////////////////

// Macro to test version of GCC.  Returns 0 for non-GCC or too old GCC.
#if defined __GNUC__ && defined __GNUC_MINOR__
#define __GNUC_PREREQ(maj, min) \
     ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#define __GNUC_PREREQ(maj, min) 0
#endif

////////////////////////////////////////////////////////////////////////////////

// Macro to test version of MSC.  Returns 0 for non-MSVC or too old MSC.
#if defined _MSC_VER
#define __MSC_PREREQ(majmin) (_MSC_VER >= (majmin))
#else
#define __MSC_PREREQ(majmin) 0
#endif

////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)

// nothing less than C++11 will suffice
#if __cplusplus < 201103L
#if (!__GNUC_PREREQ(4, 8) && !__MSC_PREREQ(1800))
#error C++11 is required
#endif
#endif

#endif

// C_ASSERT() can be used to perform many compile-time assertions:
//            type sizes, field offsets, etc.
//
#if !defined(__cplusplus) // use static_assert instead

#ifdef C_ASSERT
#undef C_ASSERT
#endif

// An assertion failure results in:
// error: size of array '__C_ASSERT__X' is negative
#define C_ASSERT_CONCAT_IMPL(x, y) x##y
#define C_ASSERT_CONCAT(x, y) C_ASSERT_CONCAT_IMPL(x, y)
#define C_ASSERT(e) \
    typedef char C_ASSERT_CONCAT(__C_ASSERT__, __COUNTER__)[(e) ? 1 : -1]

#endif

////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#define ALWAYSINLINE __forceinline
#define NEVERINLINE __declspec(noinline)
#else
#define ALWAYSINLINE inline
#define NEVERINLINE
#endif

////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#else
#define NORETURN
#endif

////////////////////////////////////////////////////////////////////////////////

#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(A) (sizeof(A)/sizeof((A)[0]))
#endif
#if !defined(NUMBER_OF)
#define NUMBER_OF(A) ARRAY_SIZE(A)
#endif
#if !defined(COUNT_OF)
#define COUNT_OF(A) ARRAY_SIZE(A)
#endif
#if !defined(NELEM)
#define NELEM(A) ARRAY_SIZE(A)
#endif

////////////////////////////////////////////////////////////////////////////////

#define IS_POW_2(n)   (((n) & (n - 1)) == 0)

////////////////////////////////////////////////////////////////////////////////

#define UNUSED(x) (void)(x)

////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#define NO_VTABLE __declspec(novtable)
#else
#define NO_VTABLE
#endif

////////////////////////////////////////////////////////////////////////////////

// The ALIGN_TO_PWR2 macro takes a size in bytes and rounds it up to a
// multiple of Pwr2 which must be a power of two.
//
//     NOTE: This macro fails for values > (SIZE_MAX - (PWR2 - 1)).
//
#define ALIGN_TO_PWR2(Size, Pwr2)  (((uintptr_t)(Size) + (Pwr2) - 1) & ~((Pwr2) - 1))

inline uintptr_t align_to_pwr2(uintptr_t to_be_aligned, uintptr_t alignment)
{
    return (to_be_aligned + alignment - 1) & ~(alignment - 1);
}
