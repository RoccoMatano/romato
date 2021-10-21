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

#include "romato.h"
#include <intrin.h>

////////////////////////////////////////////////////////////////////////////////

_CRTNOALIAS _CRTRESTRICT void* __cdecl malloc(size_t size)
{
    return HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS,
        size
        );
}

////////////////////////////////////////////////////////////////////////////////

_CRTNOALIAS void __cdecl free(void* p)
{
    HeapFree(GetProcessHeap(), 0, p);
}

////////////////////////////////////////////////////////////////////////////////

void* __cdecl operator new(size_t size)
{
    return malloc(size);
}

////////////////////////////////////////////////////////////////////////////////

void* __cdecl operator new[](size_t size)
{
    return malloc(size);
}

////////////////////////////////////////////////////////////////////////////////

void __cdecl operator delete(void* pVoid)
{
    free(pVoid);
}

////////////////////////////////////////////////////////////////////////////////

void __cdecl operator delete[](void* pVoid)
{
    free(pVoid);
}

////////////////////////////////////////////////////////////////////////////////

void operator delete(void* pVoid, size_t size)
{
    free(pVoid);
    UNUSED(size);
}

////////////////////////////////////////////////////////////////////////////////

void operator delete[](void* pVoid, size_t size)
{
    free(pVoid);
    UNUSED(size);
}

////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

namespace std
{
    void _Xinvalid_argument(char const *)
    {
        RaiseException(E_INVALIDARG);
    }

    void _Xlength_error(char const *)
    {
        RaiseException(E_BOUNDS);
    }

    void _Xout_of_range(char const *)
    {
        RaiseException(E_BOUNDS);
    }

    void _Xoverflow_error(char const *)
    {
        RaiseException(E_BOUNDS);
    }

    void _Xruntime_error(char const *)
    {
        RaiseException(E_FAIL);
    }

    void _Xbad_function_call()
    {
        RaiseException(E_ILLEGAL_METHOD_CALL);
    }

    void _Xbad_alloc()
    {
        RaiseException(E_OUTOFMEMORY);
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)

NORETURN extern "C" int _purecall()
{
    RaiseException(E_NOTIMPL);
}

#else
#error unknown compiler
#endif

////////////////////////////////////////////////////////////////////////////////

void DbgDump(const void* data, size_t len)
{
    const int bytes_per_line = 16;
    const int line_buf_size = 22 + bytes_per_line * 4;
    const int first_printable = 32; // ' ' (space)
    const int last_printable = 127; // '~'
    char line_buf[line_buf_size];

    wsprintfA(line_buf, "Dump of %p, length %Iu\n", data, len);
    OutputDebugStringA(line_buf);

    const uint8_t * const start = reinterpret_cast<const uint8_t*>(data);
    const uint8_t * const end = start + len;
    const uint8_t *chunk = start;
    while (len)
    {
        char *out = line_buf;
        out += wsprintfA(out, "%08Ix: ", chunk - start);
        for (int i = 0; i < bytes_per_line; i++)
        {
            if (chunk + i < end)
            {
                out += wsprintfA(out, "%02x", chunk[i]);
            }
            else
            {
                *out++ = ' ';
                *out++ = ' ';
            }
            *out++ = (i == (bytes_per_line / 2 - 1)) ? '|' : ' ';
        }
        *out++ = '|';
        *out++ = ' ';
        for (int i = 0; i < bytes_per_line; i++)
        {
            uint8_t c = ' ';
            if (chunk + i < end)
            {
                c = chunk[i];
                if (c < first_printable || c > last_printable)
                {
                    c = '.';
                }
            }
            *out++ = c;
        }
        chunk += bytes_per_line;
        len = (len > bytes_per_line) ? len - bytes_per_line : 0;
        *out = '\n';
        OutputDebugStringA(line_buf);
    }
}

////////////////////////////////////////////////////////////////////////////////

extern "C" const int _fltused = 0;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void* aligned_malloc(size_t size, size_t align)
{
    const size_t PTR_SIZE = sizeof(void *);

    if (!IS_POW_2(align))
    {
        return nullptr;
    }

    align = (align > PTR_SIZE ? align : PTR_SIZE) - 1;

    size_t malloc_size = PTR_SIZE + align + size;
    if (malloc_size <= size)
    {
        return nullptr;
    }

    uintptr_t alloc = p2i<uintptr_t>(malloc(malloc_size));
    if (!alloc)
    {
        return nullptr;
    }

    uintptr_t aligned = (alloc + PTR_SIZE + align) & ~align;
    (i2p<uintptr_t*>(aligned))[-1] = alloc;
    return i2p<void*>(aligned);
}

////////////////////////////////////////////////////////////////////////////////

void aligned_free(void *aligned)
{
    if (aligned)
    {
        uintptr_t* ptr = p2p<uintptr_t*>(aligned);
        free(i2p<void*>(ptr[-1]));
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define NULCHAR     TEXT('\0')
#define SPACECHAR   TEXT(' ')
#define TABCHAR     TEXT('\t')
#define QUOTECHAR   TEXT('\"')
#define BSLASHCHAR  TEXT('\\')
#define IS_SPACE(p) (*p == SPACECHAR || *p == TABCHAR)

////////////////////////////////////////////////////////////////////////////////
//
// https://docs.microsoft.com/en-us/cpp/c-language/parsing-c-command-line-arguments
//
// Microsoft C startup code uses the following rules when interpreting
// arguments given on the operating system command line:
//
//    Arguments are delimited by white space, which is either a space or a tab.
//
//    A string surrounded by double quotation marks is interpreted as a single
//    argument, regardless of white space contained within. A quoted string can
//    be embedded in an argument. Note that the caret (^) is not recognized as
//    an escape character or delimiter.
//
//    A double quotation mark preceded by a backslash, \", is interpreted as
//    a literal double quotation mark (").
//
//    Backslashes are interpreted literally, unless they immediately precede
//    a double quotation mark.
//
//    If an even number of backslashes is followed by a double quotation
//    mark, then one backslash (\) is placed in the argv array for every
//    pair of backslashes (\\), and the double quotation mark (") is
//    interpreted as a string delimiter.
//
//    If an odd number of backslashes is followed by a double quotation mark,
//    then one backslash (\) is placed in the argv array for every pair of
//    backslashes (\\) and the double quotation mark is interpreted as an
//    escape sequence by the remaining backslash, causing a literal double
//    quotation mark (") to be placed in argv.
//
// This list illustrates the rules above by showing the interpreted result
// passed to argv for several examples of command-line arguments.
//
// Command-Line Input   argv[1]     argv[2]     argv[3]
// "a b c" d e          a b c       d           e
// "ab\"c" "\\" d       ab"c        \           d
// a\\\b d"e f"g h      a\\\b       de fg       h
// a\\\"b c d           a\"b        c           d
// a\\\\"b c" d e       a\\b c      d           e
//
////////////////////////////////////////////////////////////////////////////////

static void parse_cmdl(
    PTSTR cmdl,
    PTSTR *argv,
    PTSTR args,
    int *nargs,
    int *nchars
    )
{
    int argc = 0;
    int charc = 0;

    // loop over arguments
    for (;;)
    {
        while (IS_SPACE(cmdl))
        {
            ++cmdl;
        }

        if (*cmdl == NULCHAR)
        {
            // no more args
            break;
        }

        if (argv)
        {
            *argv++ = args;
        }
        ++argc;

        // 2 * N backslashes + " -> N backslashes and begin/end quote
        // 2 * N + 1 backslashes + " -> N backslashes + literal "
        // N backslashes -> N backslashes

        bool outside_quote = true;
        // loop over chars in argument
        for (;;)
        {
            bool ignore = false;
            int nbslash = 0;

            while (*cmdl == BSLASHCHAR)
            {
                ++cmdl;
                ++nbslash;
            }

            if (*cmdl == QUOTECHAR)
            {
                if (nbslash % 2 == 0)
                {
                    if (!outside_quote && cmdl[1] == QUOTECHAR)
                    {
                        cmdl++;
                    }
                    else
                    {
                        ignore = true;
                        outside_quote = !outside_quote;
                    }
                }
                nbslash /= 2;
            }

            while (nbslash--)
            {
                if (args)
                {
                    *args++ = BSLASHCHAR;
                }
                ++charc;
            }

            if (*cmdl == NULCHAR || (outside_quote && IS_SPACE(cmdl)))
            {
                // end of argument
                break;
            }

            if (!ignore)
            {
                if (args)
                {
                    *args++ = *cmdl;
                }
                ++charc;
            }
            ++cmdl;
        }

        if (args)
        {
            *args++ = NULCHAR;
        }
        ++charc;
    }
    *nargs = argc;
    *nchars = charc;
}

////////////////////////////////////////////////////////////////////////////////

int cmdl_to_argv(PTSTR cmdl, PTSTR** ppargv)
{
    // calc size of required buffer
    int argc, nchars;
    parse_cmdl(cmdl, nullptr, nullptr, &argc, &nchars);

    // alloc buffer and parse again
    DWORD numbytes = argc * sizeof(PTSTR) + nchars * sizeof(TCHAR);
    PTSTR *pargv = static_cast<PTSTR*>(malloc(numbytes));
    if (pargv)
    {
        PTSTR args = p2p<PTSTR>(&pargv[argc]);
        parse_cmdl(cmdl, pargv, args, &argc, &nchars);
        *ppargv = pargv;
    }
    else
    {
        *ppargv = nullptr;
        argc = 0;
    }
    return argc;
}

////////////////////////////////////////////////////////////////////////////////

int get_argv(PTSTR** ppargv)
{
    TCHAR prog_name[MAX_PATH + 1];
    PTSTR cmdl = GetCommandLine();
    if (cmdl == 0 || *cmdl == NULCHAR)
    {
        GetModuleFileName(nullptr, prog_name, MAX_PATH);
        cmdl = prog_name;
    }
    return cmdl_to_argv(cmdl, ppargv);
}

////////////////////////////////////////////////////////////////////////////////

void free_argv(PTSTR* pargv)
{
    HeapFree(GetProcessHeap(), 0, pargv);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

static int find_any(PCTSTR s, PCTSTR pool)
{
    while (*s)
    {
        PCTSTR p = pool;
        while (*p && *p != *s)
        {
            ++p;
        }
        if (*p == *s) return 1;
        ++s;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

static int append_arg(PTSTR Arg, PTSTR cmdl)
{
    if (cmdl)
    {
        cmdl += sz_len(cmdl);
    }

    const int len = sz_len(Arg);
    if ((len == 0) || !find_any(Arg, TEXT(" \t\n\v\"")))
    {
        // no need to qoute
        if (cmdl)
        {
            memcpy(cmdl, Arg, len * sizeof(TCHAR));
            cmdl += len;
        }
        return len;
    }

    // have to qoute
    if (cmdl)
    {
        *cmdl++ = TEXT('"');
    }
    int res = 1;
    while (*Arg)
    {
        int NumberBackslashes = 0;
        while (*Arg == TEXT('\\'))
        {
            ++Arg;
            ++NumberBackslashes;
        }

        if (*Arg == 0)
        {
            // Escape all backslashes, but let the terminating
            // double quotation mark we add below be interpreted
            // as a metacharacter.
            const int nbs = NumberBackslashes * 2;
            if (cmdl)
            {
                for (int i = 0; i < nbs; i++)
                {
                    *cmdl++ = TEXT('\\');
                }
            }
            res += nbs;
            break;
        }
        else if (*Arg == TEXT('"'))
        {
            // Escape all backslashes and the following
            // double quotation mark.

            const int nbs = NumberBackslashes * 2 + 1;
            if (cmdl)
            {
                for (int i = 0; i < nbs; i++)
                {
                    *cmdl++ = TEXT('\\');
                }
                *cmdl++ = *Arg;
            }
            res += nbs + 1;
        }
        else
        {
            // Backslashes aren't special here.

            const int nbs = NumberBackslashes;
            if (cmdl)
            {
                for (int i = 0; i < nbs; i++)
                {
                    *cmdl++ = TEXT('\\');
                }
                *cmdl++ = *Arg;
            }
            res += nbs + 1;
        }
        ++Arg;
    }
    if (cmdl)
    {
        *cmdl++ = TEXT('"');
    }
    res += 1;
    return res;
}

////////////////////////////////////////////////////////////////////////////////

PTSTR argv_to_cmdl(int argc, PTSTR argv[])
{
    if (argc == 0)
    {
        return nullptr;
    }

    int size = argc;    // number of delimiting spaces + terminating zero
    for (int i = 0; i < argc; i++)
    {
        size += append_arg(argv[i], nullptr);
    }
    PTSTR cmdl = static_cast<PTSTR>(malloc(size * sizeof(TCHAR)));
    *cmdl = 0;
    size = 0;
    for (int i = 0; i < argc; i++)
    {
        size += append_arg(argv[i], cmdl);
        if (i < (argc - 1))
        {
            *(cmdl + size) = TEXT(' ');
            ++size;
        }
        *(cmdl + size) = 0;
    }
    return cmdl;
}

////////////////////////////////////////////////////////////////////////////////

void free_cmdl(PTSTR cmdl)
{
    HeapFree(GetProcessHeap(), 0, cmdl);
}

////////////////////////////////////////////////////////////////////////////////

PSTR* CommandLineToArgvA(PCWSTR CmdLine, int *pNumArgs)
{
    *pNumArgs = 0;
    PWSTR* argv = CommandLineToArgvW(CmdLine, pNumArgs);
    for (int i = 0; i < *pNumArgs; i++)
    {
        const UINT slen = sz_lenW(argv[i]) + 1;
        const UINT blen = slen * sizeof(WCHAR);
        PWSTR const tmp = static_cast<PWSTR>(malloc(blen));
        memcpy(tmp, argv[i], blen);
        WideCharToMultiByte(
            CP_ACP,
            0,
            tmp,
            slen,   //  num WCHARs incl. terminating null
            reinterpret_cast<PSTR>(argv[i]),
            blen,   //  num bytes incl. terminating null
            0,
            0
            );
        free(tmp);
    }
    return reinterpret_cast<PSTR*>(argv);
}

////////////////////////////////////////////////////////////////////////////////

NEVERINLINE HINSTANCE GetCallerHinstance()
{
#if defined(_MSC_VER)
    PCWSTR name = p2p<PCWSTR>(_ReturnAddress());
#else
#error unknown compiler
#endif

    HMODULE hmod = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        name,
        &hmod
        );
    return p2p<HINSTANCE>(hmod);
}

////////////////////////////////////////////////////////////////////////////////

#if defined(ROMATO_INCLUDE_SIMPLE_PRINTF) && ROMATO_INCLUDE_SIMPLE_PRINTF

extern "C" int printf(const char *fmt, ...)
{
    char Buffer[1025];
    va_list argptr;
    va_start(argptr, fmt);
    DWORD cnt = wvsprintfA(Buffer, fmt, argptr);
    va_end(argptr);
    return (
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), Buffer, cnt, &cnt, NULL) ?
        cnt :
        0
        );
}

#endif

////////////////////////////////////////////////////////////////////////////////

// just in case we should ever link with telemetry-contaminated msvcrt...

extern "C" void __telemetry_main_invoke_trigger() {}
extern "C" void __telemetry_main_return_trigger() {}
extern "C" void __vcrt_initialize_telemetry_provider() {}
extern "C" void __vcrt_uninitialize_telemetry_provider() {}

////////////////////////////////////////////////////////////////////////////////
