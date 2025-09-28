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


#include "romato.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef _CPPUNWIND
#error exception handling is NOT supported by this startup code
#endif

//////////////////////////////////////////////////////////////////////////////

#ifdef ROMATO_CONSTRUCT_STATIC_OBJECTS
#if ROMATO_CONSTRUCT_STATIC_OBJECTS
#define _STATIC_CTORS_ 1
#else
#define _STATIC_CTORS_ 0
#endif
#else
#define _STATIC_CTORS_ 0
#endif

#if _STATIC_CTORS_

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// Initializers ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

typedef void (__cdecl* P_V_FUNC_V)(void);

#ifdef _MSC_VER

#pragma section(".CRT$XCA", read)
__declspec(allocate(".CRT$XCA")) static P_V_FUNC_V CrtCppInitStart[] = { 0 };

#pragma section(".CRT$XCZ", read)
__declspec(allocate(".CRT$XCZ")) static P_V_FUNC_V CrtCppInitEnd[] = { 0 };

// merge init data with read only data
#pragma comment(linker, "/merge:.CRT=.rdata")

//////////////////////////////////////////////////////////////////////////////

static void FORCEINLINE CallInitializers()
{
    P_V_FUNC_V* pFuncStart = CrtCppInitStart + 1;
    while (pFuncStart < CrtCppInitEnd)
    {
        if (*pFuncStart != nullptr)
        {
            (**pFuncStart)();
        }
        ++pFuncStart;
    }
}

#endif

//////////////////////////////////////////////////////////////////////////////
////////////////////////////  Terminators  ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Singly linked list of exit functions

struct EXIT_ENTRY
{
    EXIT_ENTRY *Next;
    P_V_FUNC_V ExitFunc;
};

static EXIT_ENTRY ExitList; // list head

//////////////////////////////////////////////////////////////////////////////

// add a terminator

extern "C" int atexit(void(__cdecl* func)(void))
{
    auto pEE = static_cast<EXIT_ENTRY*>(malloc(sizeof(EXIT_ENTRY)));
    if (!pEE)
    {
        return 0;
    }
    pEE->Next = ExitList.Next;
    pEE->ExitFunc = func;
    ExitList.Next = pEE;
    return 1;
}

//////////////////////////////////////////////////////////////////////////////

// call terminators in LIFO order

static void FORCEINLINE CallTerminators()
{
    while (ExitList.Next != &ExitList)
    {
        EXIT_ENTRY* pEE = ExitList.Next;
        ExitList.Next = pEE->Next;
        if (pEE->ExitFunc)
        {
            pEE->ExitFunc();
        }
        free(pEE);
    }
}

#endif // _STATIC_CTORS_

//////////////////////////////////////////////////////////////////////////////
////////////////////////////  Entry Point  ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

extern "C" void EntryPoint()
{
#if _STATIC_CTORS_
    // Initializers will likely call 'atexit'. Therefore prepare
    // 'ExitList' before calling initializers.
    ExitList.Next = &ExitList;
    CallInitializers();
#endif

    const int result = rm_main();

#if _STATIC_CTORS_
    CallTerminators();
#endif

    ExitProcess(result);
}

//////////////////////////////////////////////////////////////////////////////
