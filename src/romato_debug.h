////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the romato library.
//
// Copyright 2013-2025 Rocco Matano
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

ALWAYSINLINE NORETURN void RaiseException(DWORD x)
{
    RaiseException(x, EXCEPTION_NONCONTINUABLE, 0, 0);
}

ALWAYSINLINE NORETURN void RaiseException(LONG x)
{
    RaiseException(static_cast<DWORD>(x), EXCEPTION_NONCONTINUABLE, 0, 0);
}

#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////

inline void DbgPuts(const char* str)
{
    OutputDebugStringA(str);
}
void DbgPrintf(const char* fmt, ...);
void DbgDump(const void* data, size_t len);

//////////////////////////////////////////////////////////////////////////////

#if defined(ROMATO_ACTIVATE_TRACES) && ROMATO_ACTIVATE_TRACES
#define TRACE(...) DbgPrintf(__VA_ARGS__)
#define TRACE_DUMP(d, l) DbgDump(d, l)
#else
#define TRACE(...)
#define TRACE_DUMP(d, l)
#endif

//////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#define DEBUG_BREAK __debugbreak
#else
#error unknown compiler
#endif

//////////////////////////////////////////////////////////////////////////////

#if defined(ROMATO_ACTIVATE_ASSERTS) && ROMATO_ACTIVATE_ASSERTS
    #define ASSERT(e) for(;;) { if (!(e)) { \
    DbgPrintf("%s(%d): ASSERT FAILED: '%s'\n", __FILE__, __LINE__, #e); \
    DEBUG_BREAK(); } \
    break; \
    }
#else
#define ASSERT(e)
#endif

//////////////////////////////////////////////////////////////////////////////
