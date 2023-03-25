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
#include <utility> // std::move

////////////////////////////////////////////////////////////////////////////////

class CharFromW
{
protected:

    PSTR m_str;
    UINT m_len;
    UINT m_code_page;

    NEVERINLINE CharFromW& from_w(PCWSTR p_src)
    {
        delete[] m_str;
        if (p_src)
        {
            int n = WideCharToMultiByte(
                m_code_page,
                0,
                p_src,
                -1,
                nullptr,
                0,
                nullptr,
                nullptr
                );
            m_str = new CHAR[n];
            m_len = n - 1;
            WideCharToMultiByte(
                m_code_page,
                0,
                p_src,
                -1,
                m_str,
                n,
                nullptr,
                nullptr
                );
        }
        else
        {
            m_str = nullptr;
            m_len = 0;
        }
        return *this;
    }

    NEVERINLINE CharFromW& copy(const CharFromW& src)
    {
        if (this != &src)
        {
            delete[] m_str;
            if (src.m_str)
            {
                m_str = new CHAR[src.m_len + 1];
                m_len = src.m_len;
                memcpy(m_str, src.m_str, m_len + sizeof(CHAR));
            }
            else
            {
                m_str = nullptr;
                m_len = 0;
            }
            m_code_page = src.m_code_page;
        }
        return *this;
    }

public:

    ~CharFromW()
    {
        delete[] m_str;
    }

    CharFromW(UINT cp, PCWSTR p_src) : m_str(nullptr), m_len(0), m_code_page(cp)
    {
        from_w(p_src);
    }

    CharFromW(const CharFromW& src) : m_str(nullptr)
    {
        copy(src);
    }

    CharFromW(CharFromW&& src) :
        m_str(src.m_str),
        m_len(src.m_len),
        m_code_page(src.m_code_page)
    {
        src.m_str = nullptr;
        src.m_len = 0;
    }

    CharFromW& operator=(PCWSTR p_src)
    {
        return from_w(p_src);
    }

    CharFromW& operator=(const CharFromW& src)
    {
        return copy(src);
    }

    CharFromW& operator=(CharFromW&& src)
    {
        if (m_str != src.m_str)
        {
            PSTR const s_str = m_str;
            UINT const s_len = m_len;
            UINT const s_cp = m_code_page;
            m_str = src.m_str;
            m_len = src.m_len;
            m_code_page = src.m_code_page;
            src.m_str = s_str;
            src.m_len = s_len;
            src.m_code_page = s_cp;
        }
        return *this;
    }

    operator PCSTR() const
    {
        return m_str;
    }

    PCSTR str() const
    {
        return m_str;
    }

    operator PSTR()
    {
        return m_str;
    }

    UINT length() const
    {
        return m_len;
    }

    UINT code_page() const
    {
        return m_code_page;
    }
};

////////////////////////////////////////////////////////////////////////////////

template <UINT CODE_PAGE>
class CpFromW : public CharFromW
{
    typedef CharFromW base;
public:
    CpFromW()                   : base(CODE_PAGE, nullptr)  {}
    CpFromW(PCWSTR p_src)       : base(CODE_PAGE, p_src)    {}
    CpFromW(CpFromW&& src)      : base(std::move(src))      {}
    CpFromW(const CpFromW& src) : base(src)                 {}

    CpFromW& operator=(PCWSTR p_src)
    {
        from_w(p_src);
        return *this;
    }
    CpFromW& operator=(const CpFromW& src)
    {
        base::operator=(src);
        return *this;
    }
    CpFromW& operator=(CpFromW&& src)
    {
        base::operator=(std::move(src));
        return *this;
    }
};

////////////////////////////////////////////////////////////////////////////////

typedef CpFromW<CP_ACP> AFromW;
typedef CpFromW<CP_UTF8> Utf8FromW;

////////////////////////////////////////////////////////////////////////////////
