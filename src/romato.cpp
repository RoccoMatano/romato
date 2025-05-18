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

extern "C" const int _fltused = 0;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef STATUS_NO_MEMORY
#define STATUS_NO_MEMORY ((DWORD)0xC0000017L)
#endif

void* aligned_malloc(size_t size, size_t align)
{
    const size_t PTR_SIZE = sizeof(void *);

    if (!IS_POW_2(align))
    {
        RaiseException(STATUS_NO_MEMORY);
    }

    align = (align > PTR_SIZE ? align : PTR_SIZE) - 1;

    size_t malloc_size = PTR_SIZE + align + size;
    if (malloc_size <= size)
    {
        RaiseException(STATUS_NO_MEMORY);
    }

    uintptr_t alloc = p2i<uintptr_t>(malloc(malloc_size));
    if (!alloc)
    {
        RaiseException(STATUS_NO_MEMORY);
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
// a"b"" c d            ab" c d
//
// In fact, contrary to what the above text suggests, the point is not that
// arguments must be framed with double quotation marks. Rather, the quotation
// marks switch the mode of the parser, which determines whether white space
// is interpreted as an argument separator or not. e.g:
//
// "Ar"g"um"e"n"t" With Sp"aces"" -> single argument 'Argument With Spaces'
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

        // - A string of backslashes not followed by a quotation mark has no
        //   special meaning.
        // - An even number of backslashes followed by a quotation mark is
        //   treated as pairs of protected backslashes, followed by a word
        //   terminator.
        // - An odd number of backslashes followed by a quotation mark is
        //   treated as pairs of protected backslashes, followed by a protected
        //   quotation mark.

        bool space_terminates_arg = true;
        // loop over chars in argument
        for (;;)
        {
            int cnt_backslash = 0;

            while (*cmdl == BSLASHCHAR)
            {
                ++cmdl;
                ++cnt_backslash;
            }

            bool ignore = false;
            if (*cmdl == QUOTECHAR)
            {
                if (cnt_backslash % 2 == 0)
                {
                    if (!space_terminates_arg && cmdl[1] == QUOTECHAR)
                    {
                        cmdl++;
                    }
                    else
                    {
                        // toggle mode and ignore QUOTECHAR
                        space_terminates_arg = !space_terminates_arg;
                        ignore = true;
                    }
                }
                cnt_backslash /= 2;
            }

            while (cnt_backslash--)
            {
                if (args)
                {
                    *args++ = BSLASHCHAR;
                }
                ++charc;
            }

            if (*cmdl == NULCHAR || (space_terminates_arg && IS_SPACE(cmdl)))
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

static int append_arg(PTSTR arg, PTSTR cmdl)
{
    if (cmdl)
    {
        cmdl += sz_len(cmdl);
    }

    const int len = sz_len(arg);
    // Args that are not empty and do not contain any of (space, tab, new line
    // or quotation mark) do not need to be qouted.
    if (!((len == 0) || find_any(arg, TEXT(" \t\n\""))))
    {
        if (cmdl)
        {
            memcpy(cmdl, arg, len * sizeof(TCHAR));
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
    while (*arg)
    {
        int cnt_backslash = 0;
        while (*arg == TEXT('\\'))
        {
            ++arg;
            ++cnt_backslash;
        }

        if (*arg == 0)
        {
            // Add a pair of backslashes for each encountered backslash, so
            // that the quotation mark we are going to add below will be
            // interpreted as an argument terminator.
            const int nbs = cnt_backslash * 2;
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
        else if (*arg == TEXT('"'))
        {
            // Escape all backslashes and the following double quotation mark
            // (so that there is an odd number of backslashes).
            const int nbs = cnt_backslash * 2 + 1;
            if (cmdl)
            {
                for (int i = 0; i < nbs; i++)
                {
                    *cmdl++ = TEXT('\\');
                }
                *cmdl++ = *arg;
            }
            res += nbs + 1;
        }
        else
        {
            // If we are not at the end of an argument and are not processing
            // a quotation mark, then we simply copy the backslashes to the
            // output.
            const int nbs = cnt_backslash;
            if (cmdl)
            {
                for (int i = 0; i < nbs; i++)
                {
                    *cmdl++ = TEXT('\\');
                }
                *cmdl++ = *arg;
            }
            res += nbs + 1;
        }
        ++arg;
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

extern "C" int sz_vnprintfA(PSTR buf, UINT size, PCSTR fmt, va_list args)
{
    int res = _vsnprintf(buf, size, fmt, args);
    if (res >= 0)
    {
        return res;
    }
    if (size)
    {
        buf[size - 1] = 0;
    }
    return _vsnprintf(nullptr, 0, fmt, args);
}

////////////////////////////////////////////////////////////////////////////////

extern "C" int sz_nprintfA(PSTR buf, UINT size, PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int res = sz_vnprintfA(buf, size, fmt, args);
    va_end(args);
    return res;
}

////////////////////////////////////////////////////////////////////////////////

extern "C" int sz_vnprintfW(PWSTR buf, UINT size, PCWSTR fmt, va_list args)
{
    int res = _vsnwprintf(buf, size, fmt, args);
    if (res >= 0)
    {
        return res;
    }
    if (size)
    {
        buf[size - 1] = 0;
    }
    return _vsnwprintf(nullptr, 0, fmt, args);
}

////////////////////////////////////////////////////////////////////////////////

extern "C" int sz_nprintfW(PWSTR buf, UINT size, PCWSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int res = sz_vnprintfW(buf, size, fmt, args);
    va_end(args);
    return res;
}

////////////////////////////////////////////////////////////////////////////////

void DbgPrintf(const char* fmt, ...)
{
    va_list argptr;
    char buffer[1024];

    va_start(argptr, fmt);
    sz_vnprintfA(buffer, sizeof(buffer), fmt, argptr);
    va_end(argptr);
    OutputDebugStringA(buffer);
}
////////////////////////////////////////////////////////////////////////////////

void DbgDump(const void* data, size_t len)
{
    const int bytes_per_line = 16;
    const int line_buf_size = 22 + bytes_per_line * 4;
    const int first_printable = 32; // ' ' (space)
    const int last_printable = 127; // '~'
    char line_buf[line_buf_size];

    DbgPrintf("Dump of %p, length %zu (%#zx)\n", data, len, len);

    const uint8_t * const start = reinterpret_cast<const uint8_t*>(data);
    const uint8_t * const end = start + len;
    const uint8_t *chunk = start;
    while (len)
    {
        char *out = (
            line_buf +
            sz_nprintfA(line_buf, sizeof(line_buf), "%08tx: ", chunk - start)
            );
        for (int i = 0; i < bytes_per_line; i++)
        {
            if (chunk + i < end)
            {
                uint32_t n = (chunk[i] >> 4) & 0x0f;
                *out++ = static_cast<char>(n + (n > 9 ? 'a' - 10 : '0'));
                n = chunk[i] & 0x0f;
                *out++ = static_cast<char>(n + (n > 9 ? 'a' - 10 : '0'));
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
        *out++ = '\n';
        *out = '\0';
        OutputDebugStringA(line_buf);
    }
}

////////////////////////////////////////////////////////////////////////////////

#if defined(ROMATO_INCLUDE_SIMPLE_PRINTF) && ROMATO_INCLUDE_SIMPLE_PRINTF

extern "C" int printf(const char *fmt, ...)
{
    char buf[1025];
    va_list args;
    va_start(args, fmt);
    int res = sz_vnprintfA(buf, sizeof(buf), fmt, args);
    va_end(args);
    DWORD cnt = res < sizeof(buf) ? res : sizeof(buf) - 1;
    return (
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, cnt, &cnt, NULL) ?
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
