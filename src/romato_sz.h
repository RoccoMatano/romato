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

template<typename T> UINT inline sz_lenT(const T* str)
{
    UINT len = ~0U;
    do
    {
        ++len;
    }
    while (str[len] != 0);
    return len;
}

extern "C" inline UINT sz_lenW(PCWSTR str)
{
    return sz_lenT<WCHAR>(str);
}

extern "C" inline UINT sz_lenA(PCSTR str)
{
    return sz_lenT<CHAR>(str);
}

#ifdef UNICODE
#define sz_len sz_lenW
#else
#define sz_len sz_lenA
#endif

////////////////////////////////////////////////////////////////////////////////

template<typename T> UINT inline sz_nlenT(const T* str, UINT maxlen)
{
    const T* it = str;
    const T* const end = it + maxlen;
    while(it != end && *it != 0)
    {
        it++;
    }
    return static_cast<UINT>(it - str);
}

extern "C" inline UINT sz_nlenW(PCWSTR str, UINT maxlen)
{
    return sz_nlenT<WCHAR>(str, maxlen);
}

extern "C" inline UINT sz_nlenA(PCSTR str, UINT maxlen)
{
    return sz_nlenT<CHAR>(str, maxlen);
}

#ifdef UNICODE
#define sz_nlen sz_nlenW
#else
#define sz_nlen sz_nlenA
#endif

////////////////////////////////////////////////////////////////////////////////

template<typename T> inline T* sz_cpyT(T* dst, const T* src)
{
    T* const result = dst;
    T c;
    do
    {
        c = *dst++ = *src++;
    }
    while (c != 0);
    return result;
}

extern "C" PWSTR inline sz_cpyW(PWSTR dst, PCWSTR src)
{
    return sz_cpyT<WCHAR>(dst, src);
}

extern "C" PSTR inline sz_cpyA(PSTR dst, PCSTR src)
{
    return sz_cpyT<CHAR>(dst, src);
}

#ifdef UNICODE
#define sz_cpy sz_cpyW
#else
#define sz_cpy sz_cpyA
#endif

////////////////////////////////////////////////////////////////////////////////

template<typename T> T* sz_catT(T* dst, const T* src)
{
    T* const result = dst;
    while (*dst)
    {
        dst++;
    }
    T c;
    do
    {
        c = *dst++ = *src++;
    }
    while (c != 0);
    return result;
}

extern "C" PWSTR inline sz_catW(PWSTR dst, PCWSTR src)
{
    return sz_catT<WCHAR>(dst, src);
}

extern "C" PSTR inline sz_catA(PSTR dst, PCSTR src)
{
    return sz_catT<CHAR>(dst, src);
}

#ifdef UNICODE
#define sz_cat sz_catW
#else
#define sz_cat sz_catA
#endif

////////////////////////////////////////////////////////////////////////////////

// Always terminate destination with zero (like lstrcpyn).
// Of course we cannot do that if len == 0.

template<typename T> T* sz_cpynT(T* dst, const T* src, UINT len)
{
    T* const result = dst;
    if (len != 0)
    {
        while (len--)
        {
            const T c = *dst++ = *src++;
            if (c == 0) return result;
        }
        *(--dst) = 0;
    }
    return result;

}

extern "C" PWSTR inline sz_cpynW(PWSTR dst, PCWSTR src, UINT len)
{
    return sz_cpynT<WCHAR>(dst, src, len);
}

extern "C" PSTR inline sz_cpynA(PSTR dst, PCSTR src, UINT len)
{
    return sz_cpynT<CHAR>(dst, src, len);
}

#ifdef UNICODE
#define sz_cpyn sz_cpynW
#else
#define sz_cpyn sz_cpynA
#endif

////////////////////////////////////////////////////////////////////////////////

// Only terminate destination with zero, if len allows to copy the zero from
// source (like strncpy). Pad destination with zero if source was shorter.

template<typename T> T* sz_ncpyT(T* dst, const T* src, UINT len)
{
    T* const result = dst;
    while (len-- && (*dst++ = *src++) != 0);
    while (len--)
    {
        *dst++ = 0;
    }
    return result;
}

extern "C" PWSTR inline sz_ncpyW(PWSTR dst, PCWSTR src, UINT len)
{
    return sz_ncpyT<WCHAR>(dst, src, len);
}

extern "C" PSTR inline sz_ncpyA(PSTR dst, PCSTR src, UINT len)
{
    return sz_ncpyT<CHAR>(dst, src, len);
}

#ifdef UNICODE
#define sz_ncpy sz_ncpyW
#else
#define sz_ncpy sz_ncpyA
#endif

////////////////////////////////////////////////////////////////////////////////

template<typename T> T* sz_ncatT(T* dst, const T* src, UINT size_dst)
{
    const UINT len = sz_lenT<T>(dst);
    if (size_dst > len)
    {
        sz_cpynT<T>(dst + len, src, size_dst - len);
    }
    return dst;
}

extern "C" PWSTR inline sz_ncatW(PWSTR dst, PCWSTR src, UINT size_dst)
{
    return sz_ncatT<WCHAR>(dst, src, size_dst);
}

extern "C" PSTR inline sz_ncatA(PSTR dst, PCSTR src, UINT size_dst)
{
    return sz_ncatT<CHAR>(dst, src, size_dst);
}

#ifdef UNICODE
#define sz_ncat sz_ncatW
#else
#define sz_ncat sz_ncatA
#endif


////////////////////////////////////////////////////////////////////////////////

template<typename T> inline T* sz_strT(const T* searchee, const T* lookfor)
{
    if (!*lookfor)
    {
        return const_cast<T*>(searchee);
    }

    while (*searchee)
    {
        const T* s = searchee;
        const T* l = lookfor;

        while (*s && *l && (*s - *l) == 0)
        {
            s++;
            l++;
        }

        if (!*l)
        {
            return const_cast<T*>(searchee);
        }
        searchee++;
    }

    return nullptr;
}

extern "C" inline PWSTR sz_strW(PCWSTR searchee, PCWSTR lookfor)
{
    return sz_strT<WCHAR>(searchee, lookfor);
}

extern "C" inline PSTR sz_strA(PCSTR searchee, PCSTR lookfor)
{
    return sz_strT<CHAR>(searchee, lookfor);
}

#ifdef UNICODE
#define sz_str sz_strW
#else
#define sz_str sz_strA
#endif

////////////////////////////////////////////////////////////////////////////////

template<typename T> inline T* sz_chrT(const T* searchee, int ch)
{
    const T cch = static_cast<T>(ch);
    while (*searchee && *searchee != cch)
    {
        searchee++;
    }
    return *searchee == cch ? const_cast<T*>(searchee) : nullptr;
}

extern "C" inline PWSTR sz_chrW(PCWSTR searchee, int ch)
{
    return sz_chrT<WCHAR>(searchee, ch);
}

extern "C" inline PSTR sz_chrA(PCSTR searchee, int ch)
{
    return sz_chrT<CHAR>(searchee, ch);
}

#ifdef UNICODE
#define sz_chr sz_chrW
#else
#define sz_chr sz_chrA
#endif

////////////////////////////////////////////////////////////////////////////////

template<typename T> int sz_cmpT(const T* a, const T* b)
{
    // any value != 0 should be considered > 0
    static_assert(std::is_unsigned<T>::value, "must be unsigned");
    int result = 0;
    while ((result = *a - *b) == 0 && *b)
    {
        ++a;
        ++b;
    }
    return result;
}

extern "C" int inline sz_cmpW(PCWSTR a, PCWSTR b)
{
    // WCHAR is either a typedef for 'unsigned short' or it is wchar_t,
    // which is a distict type, but on Windows is also unsigned.
    return sz_cmpT<WCHAR>(a, b);
}

extern "C" int inline sz_cmpA(PCSTR a, PCSTR b)
{
    // make sure sz_cmpT operates on unsigned values
    return sz_cmpT<BYTE>(
        reinterpret_cast<const BYTE*>(a),
        reinterpret_cast<const BYTE*>(b)
        );
}

#ifdef UNICODE
#define sz_cmp sz_cmpW
#else
#define sz_cmp sz_cmpA
#endif

////////////////////////////////////////////////////////////////////////////////

extern "C" int inline sz_cmpiW(PCWSTR a, PCWSTR b)
{
    return (
        CompareStringW(
            LOCALE_USER_DEFAULT,
            SORT_STRINGSORT | NORM_IGNORECASE,
            a,
            -1,
            b,
            -1
            )
        - CSTR_EQUAL
        );
}

extern "C" int inline sz_cmpiA(PCSTR a, PCSTR b)
{
    return (
        CompareStringA(
            LOCALE_USER_DEFAULT,
            SORT_STRINGSORT | NORM_IGNORECASE,
            a,
            -1,
            b,
            -1
            )
        - CSTR_EQUAL
        );
}

#ifdef UNICODE
#define sz_cmpi sz_cmpiW
#else
#define sz_cmpi sz_cmpiA
#endif

////////////////////////////////////////////////////////////////////////////////

extern "C" int sz_vnprintfA(PSTR buf, UINT size, PCSTR fmt, va_list args);
extern "C" int sz_vnprintfW(PWSTR buf, UINT size, PCWSTR fmt, va_list args);
extern "C" int sz_nprintfA(PSTR buf, UINT size, PCSTR fmt, ...);
extern "C" int sz_nprintfW(PWSTR buf, UINT size, PCWSTR fmt, ...);

#ifdef UNICODE
#define sz_vnprintf sz_vnprintfW
#define sz_nprintf sz_nprintfW
#define SZ_PRINTF(buf, ...) sz_nprintfW(buf, ARRAY_SIZE(buf), __VA_ARGS__)
#else
#define sz_vnprintf sz_vnprintfA
#define sz_nprintf sz_nprintfA
#define SZ_PRINTF(buf, ...) sz_nprintfA(buf, ARRAY_SIZE(buf), __VA_ARGS__)
#endif

#define SZ_PRINTFA(buf, ...) sz_nprintfA(buf, ARRAY_SIZE(buf), __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
