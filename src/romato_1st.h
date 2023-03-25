////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the romato library.
//
// Copyright 2013-2023 Rocco Matano
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

#ifdef _WINDOWS_
#error WINDOWS.H already included.  ROMATO apps must not #include <windows.h>
#endif

// make the compiler ignore any linkage definitions from the crt headers
#define _CRTIMP
#define _CRTIMP2_PURE
#define _VCRTIMP

// disable _SECURE_SCL checks
#define _SCL_SECURE_NO_WARNINGS 1
#define _SCL_SECURE_NO_DEPRECATE 1
#define _SCL_SECURE_INVALID_PARAMETER(x)

// disable exceptions in STL
#define _HAS_EXCEPTIONS 0

// disable iterator debugging
#define _ITERATOR_DEBUG_LEVEL 0

// suppress stdio inline functions, so they can be redefined
#define _CRT_SECURE_NO_WARNINGS
#define _NO_CRT_STDIO_INLINE
#define __USE_MINGW_ANSI_STDIO 0

// exclude rarely used and ancient stuff
#define WIN32_LEAN_AND_MEAN

// windows.h will eventually include stdlib.h. If this file is compiled by g++,
// the version from the c++ include directory will be used. That will include
// cstdlib and ultimately include os_defines.h. os_defines.h UNCONDITIONALLY
// redefines __USE_MINGW_ANSI_STDIO as 1!
// This can be inhibited by defining _GLIBCXX_INCLUDE_NEXT_C_HEADERS. In that
// case stdlib.h from the c++ directory will do a #include_next on stdlib.h
// which will include the version from the c directory.
#define _GLIBCXX_INCLUDE_NEXT_C_HEADERS
#include <windows.h>
// since we defined WIN32_LEAN_AND_MEAN, we have to include shellapi.h
// explicitly.
#include <shellapi.h>
#include <commctrl.h>
// windows.h will only include stdlib.h, if WIN32_LEAN_AND_MEAN is not defined.
// So we include stdlib.h here, just in case that should be defined.
#include <stdlib.h>
#undef _GLIBCXX_INCLUDE_NEXT_C_HEADERS

#include <stdio.h>
#include <stdint.h>
