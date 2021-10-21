////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the romato library.
//
// Copyright 2013-2021 Rocco Matano
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

////////////////////////////////////////////////////////////////////////////////
//
// MSVC considers memset, memcpy and memcmp as 'compiler internal functions'
// that must not be redefined in code that is compiled with 'link time code
// generation'. Therefore these are defined here, so that this file can be
// the only one that is compiled without 'ltcg'.
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
// Make the compiler ignore any linkage definitions from the crt headers,
// since we are going to define functions from the crt.
#define _CRTIMP
#define _VCRTIMP
#if _MSC_VER < 1800 && !defined(nullptr)
#define nullptr NULL
#endif
#endif

#if !(defined(_M_IX86) || defined(__i386)) && !(defined(_M_X64) || defined(__amd64))
#error Unsupported platform
#endif

#include <string.h>
#include <intrin.h>

#ifdef _MSC_VER
// It seems that for MSVC memcmp can be an intrinsic function for _MSC_VER
// < 1600 whereas memset and memcpy never will be.
#if _MSC_VER < 1600
#pragma intrinsic(memcmp)
#define _NEED_MEM_CMP_FUNC_ 0
#else
#pragma function(memcmp)
#define _NEED_MEM_CMP_FUNC_ 1
#endif
#pragma function(memset)
#pragma function(memcpy)
#else
#define _NEED_MEM_CMP_FUNC_ 1
#endif

////////////////////////////////////////////////////////////////////////////////

extern "C" void* memset(void* dst, int val, size_t count)
{
    unsigned char* cdst = static_cast<unsigned char*>(dst);
    __stosb(cdst, static_cast<unsigned char>(val), count);
    return dst;
}

////////////////////////////////////////////////////////////////////////////////

extern "C" void* memcpy(void* dst, const void* src, size_t count)
{
    unsigned char* cdst = static_cast<unsigned char*>(dst);
    const unsigned char* csrc = static_cast<const unsigned char*>(src);
    __movsb(cdst, csrc, count);
    return dst;
}

////////////////////////////////////////////////////////////////////////////////

extern "C" void* memmove(void* dst, const void* src, size_t count)
{
    unsigned char* cdst = static_cast<unsigned char*>(dst);
    const unsigned char* csrc = static_cast<const unsigned char*>(src);
    const ptrdiff_t diff = csrc - cdst;
    if ((diff >= 0) || (diff + count <= 0))
    {
        return memcpy(dst, src, count);
    }
    cdst += count - 1;
    csrc += count - 1;
    const uintptr_t flags = __readeflags();
    __writeeflags(flags | 0x400);   // set direction flag
    __movsb(cdst, csrc, count);
    __writeeflags(flags);           // restore direction flag
    return dst;
}

////////////////////////////////////////////////////////////////////////////////

#if _NEED_MEM_CMP_FUNC_

extern "C" int memcmp(void const* p1, void const* p2, size_t count)
{
    const unsigned char* c1 = static_cast<const unsigned char*>(p1);
    const unsigned char* c2 = static_cast<const unsigned char*>(p2);
    const unsigned char* const end = c1 + count;
    while (c1 < end)
    {
        int d = *c1++ - *c2++;
        if (d)
        {
            return d;
        }
    }
    return 0;
}

#endif

////////////////////////////////////////////////////////////////////////////////

extern "C" const void * memchr(const void* buf, int chr,size_t cnt)
{
    const unsigned char* ubuf = reinterpret_cast<const unsigned char*>(buf);
    unsigned char uchr = static_cast<unsigned char>(chr);
    while (cnt && (*ubuf != uchr))
    {
        ubuf++;
        cnt--;
    }
    return (cnt ? reinterpret_cast<const void*>(ubuf) : nullptr);
}

////////////////////////////////////////////////////////////////////////////////
