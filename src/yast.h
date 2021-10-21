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
// No this is NOT S.u.S.E. Linux's setup tool. It's
//
// ##  ##           ##
// ##  ##           ##
// ##  ##   ####   #####
//  ####   ##  ##   ##
//   ##    ######   ##
//   ##    ##       ##
//   ##     ####     ###
//
//                          ##     ##
//                          ##     ##
//  ####   #####    ####   #####   #####    ####   ## ##
//     ##  ##  ##  ##  ##   ##     ##  ##  ##  ##  ### ##
//  #####  ##  ##  ##  ##   ##     ##  ##  ######  ##
// ##  ##  ##  ##  ##  ##   ##     ##  ##  ##      ##
//  #####  ##  ##   ####     ###   ##  ##   ####   ##
//
//          ##               ##
//          ##
//  #####  #####   ## ##    ###    #####    #####
// ##       ##     ### ##    ##    ##  ##  ##  ##
//  ####    ##     ##        ##    ##  ##  ##  ##
//     ##   ##     ##        ##    ##  ##   #####
// #####     ###   ##       ####   ##  ##      ##
//                                          ####
//  ##
//  ##
// #####   ##  ##  #####    ####
//  ##     ##  ##  ##  ##  ##  ##
//  ##     ##  ##  ##  ##  ######
//  ##      #####  #####   ##
//   ###       ##  ##       ####
//          ####   ##
//
////////////////////////////////////////////////////////////////////////////////
//
// As its basis Yast is using an allocation and representation scheme that is
// very similar to BSTR. In fact Yast started out as a simple wrapper for BSTR.
// But after using that wrapper for some time it became obvious, that one
// particular trait of BSTR is quite repellent. It is the fact that BSTR is
// allowed to represent an empty string by using nullptr. So there is almost
// nothing that you can do with a BSTR before checking whether it is nullptr
// or not. That is the reason why Yast decided: "Memory is cheap enough to
// allocate a buffer even for an empty string".
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "container.h"

////////////////////////////////////////////////////////////////////////////////

class Yast;
typedef cvector<Yast> YastVector;

class Yast
{
protected:

    ////////////////////////////////////////////////////////////////////////////

    typedef WCHAR* YSTR;

    static YSTR allocate_bytes(const void* str, UINT length);

    static inline YSTR allocate(PCWSTR str, UINT length)
    {
        return allocate_bytes(str, length * sizeof(WCHAR));
    }

    static inline void release(YSTR str)
    {
        if (str)
        {
            void **release_me = reinterpret_cast<void**>(str) - 1;
            free(release_me);
        }
    }

    static YSTR from_char(PCSTR p_str, int length, UINT code_page);

    YSTR m_str;

    ////////////////////////////////////////////////////////////////////////////

public:

    static const UINT MAX_BYTE_LEN = UINT_MAX - 127;
    static const UINT MAX_LEN = MAX_BYTE_LEN / sizeof(WCHAR);

    UINT byte_length() const
    {
        uint32_t *p_length = reinterpret_cast<uint32_t*>(m_str) - 1;
        return *p_length;
    }

    UINT length() const
    {
        uint32_t *p_length = reinterpret_cast<uint32_t*>(m_str) - 1;
        return *p_length / sizeof(WCHAR);
    }

    ~Yast()
    {
        release(m_str);
    }

    ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////// constructors /////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    Yast()
        : m_str(allocate(nullptr, 0))
    {
    }

    Yast(const Yast& src)
        : m_str(allocate_bytes(src.m_str, src.byte_length()))
    {
    }

    Yast(PCWSTR p_src)
        : m_str(allocate(p_src, p_src ? sz_lenW(p_src) : 0))
    {
    }

    Yast(Yast&& src)
        : m_str(src.m_str)
    {
        // Here is the only place where we set m_str to nullptr. On the one
        // hand we have to do this to keep the dtor for 'src' from freeing
        // the string we just claimed. And on the other hand client code can
        // either not see this (because src is a temporary object) or has
        // explicitly requested it (using std::move).
        src.m_str = nullptr;
    }

    explicit Yast(UINT size)
        : m_str(allocate(nullptr, size))
    {
    }

    // Constructing a Yast of lenght 'size' and using the first 'size'
    // characters from 'p_str' to initialize it -> p_str must be AT LEAST
    // 'size' long, even beyond embedded zeros!
    Yast(PCWSTR p_str, UINT size)
        : m_str(allocate(p_str, size))
    {
    }

    // Constructing a Yast of lenght 'size' and using the characters from
    // 'src' to initialize it. The number of characters from 'src' that are
    // used for initialization is min(size, src.length()).
    Yast(const Yast& src, UINT size)
        : m_str(allocate(nullptr, size))
    {
        const UINT byte_size = size * sizeof(WCHAR);
        UINT blen = src.byte_length();
        if (blen > byte_size)
        {
            blen = byte_size;
        }
        memcpy(m_str, src.m_str, blen);
    }

    // This is the ctor that will be used, when a Yast is constructed from
    // a 'char*', e.g. Yast("Hello world!"). By default the ANSI code page
    // is used to convert to unicode.
    Yast(PCSTR p_src, UINT code_page = CP_ACP)
        : m_str(
            p_src != nullptr ?
            from_char(p_src, -1, code_page) :
            allocate(nullptr, 0)
            )
    {
    }

    Yast(UINT size, PCSTR p_str, UINT code_page = CP_ACP)
        : m_str(
            size == 0 || p_str == nullptr ?
            allocate(nullptr, size) :
            from_char(p_str, size, code_page)
            )
    {
    }

    // Construct a Yast with WM_GETTEXT
    Yast(HWND hWnd);

    ////////////////////////////////////////////////////////////////////////////
    /////////////////////////////// casting ////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    operator PCWSTR() const
    {
        return m_str;
    }

    PCWSTR str() const
    {
        return m_str;
    }

    operator PWSTR()
    {
        return m_str;
    }

    // Convienience cast to make it easier to pass a Yast to SendMessage etc.
    operator LPARAM()
    {
        return reinterpret_cast<LPARAM>(m_str);
    }

    ////////////////////////////////////////////////////////////////////////////
    /////////////////////////// length handling ////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    bool is_empty() const
    {
        return (byte_length() == 0);
    }

    bool operator !()  const
    {
        return byte_length() == 0;
    }

    bool is_empty_sz() const
    {
        return (*m_str == 0);
    }

    UINT sz_length() const
    {
        return sz_lenW(m_str);
    }

    Yast& clear(UINT size = 0)
    {
        release(m_str);
        m_str = allocate(nullptr, size);
        return *this;
    }

    // Adjust the length to the first embedded zero if there is any.
    Yast& adapt_to_sz()
    {
        const UINT szlen = sz_lenW(m_str);
        if (szlen != length())
        {
            YSTR tmp = allocate(m_str, szlen);
            release(m_str);
            m_str = tmp;
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    //////////////////////// assignment operators //////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    Yast& operator=(const Yast& src)
    {
        return copy_from(src);
    }

    Yast& operator=(PCWSTR p_src)
    {
        return copy_from(p_src);
    }

    Yast& operator=(Yast&& src)
    {
        if (m_str != src.m_str)
        {
            YSTR const str = m_str;
            m_str = src.m_str;
            src.m_str = str;
        }
        return *this;
    }

    // Interpreting source as ANSI.
    Yast& operator=(PCSTR p_src)
    {
        release(m_str);
        m_str = from_char(p_src, -1, CP_ACP);
        return *this;
    }

    // Interpreting source as UTF-8. Since ANSI and UTF-8 strings share the
    // same type, this cannot be an assignment operator.
    Yast& from_utf8(PCSTR p_src)
    {
        release(m_str);
        m_str = from_char(p_src, -1, CP_UTF8);
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// iterators ///////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    using iterator               = array_iterator<WCHAR>;
    using const_iterator         = array_const_iterator<WCHAR>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin()
    {
        return iterator(m_str);
    }

    const_iterator begin() const
    {
        return const_iterator(m_str);
    }

    iterator end()
    {
        return iterator(m_str + length());
    }

    const_iterator end() const
    {
        return const_iterator(m_str + length());
    }

    const_iterator cbegin() const
    {
        return begin();
    }

    const_iterator cend() const
    {
        return end();
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const
    {
        return rbegin();
    }

    const_reverse_iterator crend() const
    {
        return rend();
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// utilities ///////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    Yast& to_lower()
    {
        CharLowerBuffW(m_str, length());
        return *this;
    }

    Yast& to_upper()
    {
        CharUpperBuffW(m_str, length());
        return *this;
    }

    Yast& copy_from(const Yast& src)
    {
        if (m_str != src.m_str)
        {
            release(m_str);
            m_str = allocate_bytes(src.m_str, src.byte_length());
        }
        return *this;
    }

    Yast& copy_from(PCWSTR p_src, UINT len)
    {
        if (m_str != p_src)
        {
            release(m_str);
            m_str = allocate(p_src, len);
        }
        return *this;
    }

    Yast& copy_from(PCWSTR p_src)
    {
        return copy_from(p_src, p_src ? sz_lenW(p_src) : 0);
    }

    // Absorb arbitrary data for mere transportation purposes.
    void absorb(const void *p_data, UINT len)
    {
        if (p_data != m_str)
        {
            release(m_str);
            m_str = allocate_bytes(p_data, len);
        }
    }

    bool load_res_string(
        UINT str_id,
        HINSTANCE h_inst = GetCallerHinstance(),
        UINT lang_id = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)
        );

    // Python like slicing, with the difference that if the end index is
    // negative it does NOT denote the first character that not a part of the
    // slice, but the last that IS part of the slice.
    //
    // Python                     | Yast
    // ---------------------------+----------------------------------
    // s = "Hello world"          | Yast s("Hello world")
    // s[:]     -> "Hello world"  | s.slice(0, -1)   -> "Hello world"
    // s[1:-3]  -> "ello wo"      | s.slice(1, -4)   -> "ello wo"
    // s[-10:8] -> "ello wo"      | s.slice(-10, 8)  -> "ello wo"
    // s[-99:]  -> "Hello world"  | s.slice(-99, -1) -> "Hello world"
    // s[:99]   -> "Hello world"  | s.slice(0, 99)   -> "Hello world"
    Yast slice(int begin, int end) const;

    Yast substr(int begin, int length) const
    {
        return slice(begin, begin + length);
    }

    // These are restricted to the capabilities of wsprintf (e.g. no support
    // for floating point).
    Yast& format(PCSTR fmt, ...);
    Yast& format(PCWSTR fmt, ...);

    UINT adopt_wnd_text(HWND hWnd);

    Yast& expand_env_vars();

    Yast& reverse();

    size_t hash() const;

    bool to_clipboard(HWND wnd = nullptr) const;
    bool from_clipboard(HWND wnd = nullptr);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// searching ///////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    int find(int len, PCWSTR what, int offset = 0) const;

    int find(PCWSTR what, int offset = 0) const
    {
        return find(sz_lenW(what), what, offset);
    }

    int find(const Yast& what, int offset = 0) const
    {
        return find(what.length(), what.m_str, offset);
    }

    int rfind(PCWSTR what, int len) const;

    int rfind(PCWSTR what) const
    {
        return rfind(what, sz_lenW(what));
    }

    int rfind(const Yast& what) const
    {
        return rfind(what.m_str, what.length());
    }

    YastVector split(PCWSTR seperator) const;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// replacing ///////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    // in place
    Yast& replace(const Yast& what, const Yast& replacement);

    // out of place
    Yast replaced(const Yast& what, const Yast& replacement) const
    {
        return Yast(*this).replace(what, replacement);
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// appending ///////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    Yast& append(PCWSTR p_str, UINT len);

    Yast& operator+=(PCWSTR p_src)
    {
        return append(p_src, sz_lenW(p_src));
    }

    Yast& operator+=(const Yast& src)
    {
        return append(src.m_str, src.length());
    }

    friend Yast operator+(const Yast& s1, const Yast& s2)
    {
        return Yast(s1).append(s2.m_str, s2.length());
    }

    friend Yast operator+(const Yast& s1, PCWSTR s2)
    {
        return Yast(s1).append(s2, sz_lenW(s2));
    }

    friend Yast operator+(PCWSTR s1, const Yast& s2)
    {
        return Yast(s1).append(s2.m_str, s2.length());
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////// comparison //////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    friend int compare(const Yast& a, const Yast& b)
    {
        return CompareStringW(
            LOCALE_USER_DEFAULT,
            0,
            a.m_str,
            a.length(),
            b.m_str,
            b.length()
            );
    }

    // performance optimized test for binary equality
    bool binary_same(const Yast& cmp) const;

    bool operator==(const Yast& cmp) const
    {
        return length() == cmp.length() && compare(*this, cmp) == CSTR_EQUAL;
    }

    bool operator==(PCWSTR p_cmp) const
    {
        return operator==(Yast(p_cmp));
    }

    bool operator==(PWSTR p_cmp) const
    {
        return operator==(static_cast<PCWSTR>(p_cmp));
    }

    bool operator==(PCSTR p_cmp) const
    {
        return operator==(Yast(p_cmp));
    }

    bool startswith(const Yast& prefix) const
    {
        Yast self_prefix(slice(0, prefix.length()));
        return self_prefix == prefix;
    }

    bool startswith(PCWSTR prefix) const
    {
        return startswith(Yast(prefix));
    }

    bool startswith(PCSTR prefix) const
    {
        return startswith(Yast(prefix));
    }

    bool endswith(const Yast& suffix) const
    {
        Yast self_suffix(slice(-static_cast<int>(suffix.length()), -1));
        return self_suffix == suffix;
    }

    bool endswith(PCWSTR suffix) const
    {
        return endswith(Yast(suffix));
    }

    bool endswith(PCSTR suffix) const
    {
        return endswith(Yast(suffix));
    }

    bool operator!=(const Yast& cmp) const
    {
        return !operator==(cmp);
    }

    bool operator!=(PCWSTR p_cmp) const
    {
        return !operator==(p_cmp);
    }

    bool operator!=(PWSTR p_cmp) const
    {
        return operator!=(static_cast<PCWSTR>(p_cmp));
    }

    bool operator!=(PCSTR p_cmp) const
    {
        return !operator==(p_cmp);
    }

    bool operator<(const Yast& cmp) const
    {
        return compare(*this, cmp) == CSTR_LESS_THAN;
    }

    bool operator<(PCWSTR p_cmp) const
    {
        return operator<(Yast(p_cmp));
    }

    bool operator<(PWSTR p_cmp) const
    {
        return operator<(static_cast<PCWSTR>(p_cmp));
    }

    bool operator<(PCSTR p_cmp) const
    {
        return operator<(Yast(p_cmp));
    }

    bool operator>(const Yast& cmp) const
    {
        return compare(*this, cmp) == CSTR_GREATER_THAN;
    }

    bool operator>(PCWSTR p_cmp) const
    {
        return operator>(Yast(p_cmp));
    }

    bool operator>(PWSTR p_cmp) const
    {
        return operator>(static_cast<PCWSTR>(p_cmp));
    }

    bool operator>(PCSTR p_cmp) const
    {
        return operator>(Yast(p_cmp));
    }
};

static_assert(sizeof(Yast) == sizeof(void*), "Unexpected size of Yast");

////////////////////////////////////////////////////////////////////////////////

// Inject specialization of std::hash into namespace std, so that Yast can be
// used as the key type of std::unordered_map.
#include <functional>
namespace std
{
    template<> struct hash<Yast>
    {
        size_t operator()(Yast const& s) const noexcept
        {
            return s.hash();
        }
    };
}

////////////////////////////////////////////////////////////////////////////////
