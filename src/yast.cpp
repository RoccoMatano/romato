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

#include "romato.h"
#include "yast.h"
#include "string_res.h"

////////////////////////////////////////////////////////////////////////////////

Yast::YSTR Yast::allocate_bytes(const void* str, UINT length)
{
    // Add space for storing length and terminating 0, then align.
    const UINT alignment = 16;
    const UINT mask = alignment - 1;
    const UINT add_len = sizeof(uintptr_t) + sizeof(WCHAR) + mask;
    if (length > MAX_BYTE_LEN)
    {
        length = MAX_BYTE_LEN;
    }
    const UINT alloc_len = (length + add_len) & ~mask;

    // Allocate memory and store length.
    PSTR p = static_cast<PSTR>(malloc(alloc_len));
    uint32_t *p_length = p2p<uint32_t*>(
        p + sizeof(uintptr_t) - sizeof(uint32_t)
        );
    *p_length = length;

    // Copy init data.
    PSTR res = p + sizeof(uintptr_t);
    if (str)
    {
        memcpy(res, str, length);
    }

    // Write CHAR terminator.
    *(res + length) = 0;
    // Also write WCHAR terminator, taking alignment into account. The way
    // alloc_len is calculated ensures, that we do NOT overflow our buffer.
    *(p2p<PWSTR>(res + (~1 & (length + 1)))) = 0;

    return p2p<YSTR>(res);
}

////////////////////////////////////////////////////////////////////////////////

Yast::YSTR Yast::from_char(PCSTR p_str, int length, UINT code_page)
{
    if (p_str == nullptr || length == 0)
    {
        return allocate(nullptr, 0);
    }

    int cvt_len = MultiByteToWideChar(code_page, 0, p_str, length, nullptr, 0);
    int alloc_len = cvt_len;
    if (length == -1)
    {
        // Reduce alloc_len by one, since allocate will implicitly add space
        // for terminating '\0'.
        alloc_len -= 1;
    }
    YSTR str = allocate(nullptr, alloc_len);
    MultiByteToWideChar(code_page, 0, p_str, length, str, cvt_len);
    return str;
}

////////////////////////////////////////////////////////////////////////////////

Yast::Yast(HWND hWnd)
{
    // Length excluding terminating null character.
    const UINT len = static_cast<UINT>(
        SendMessageW(hWnd, WM_GETTEXTLENGTH, 0, 0)
        );
    m_str = allocate(nullptr, len);
    // Length including terminating null character.
    SendMessageW(hWnd, WM_GETTEXT, len + 1, *this); // operator LPARAM()
}

////////////////////////////////////////////////////////////////////////////////

Yast& Yast::format(PCSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    UINT needed = sz_vnprintfA(nullptr, 0, fmt, args);
    PSTR y = reinterpret_cast<PSTR>(allocate_bytes(nullptr, needed));
    sz_vnprintfA(y, needed + 1, fmt, args);
    va_end(args);
    m_str = from_char(y, needed, CP_ACP);
    release(reinterpret_cast<YSTR>(y));
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

Yast& Yast::format(PCWSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    UINT needed = sz_vnprintfW(nullptr, 0, fmt, args);
    release(m_str);
    m_str = allocate(nullptr, needed);
    sz_vnprintfW(m_str, needed + 1, fmt, args);
    va_end(args);
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

UINT Yast::adopt_wnd_text(HWND hWnd)
{
    *this = Yast(hWnd);
    return length();
}

////////////////////////////////////////////////////////////////////////////////

bool Yast::load_res_string(UINT str_id, HINSTANCE h_inst, UINT lang_id)
{
    StringResourceEntry* pSRE = FindStringResource(h_inst, str_id, lang_id);
    if (pSRE != nullptr)
    {
        release(m_str);
        m_str = allocate(pSRE->m_string, pSRE->m_length);
    }
    return pSRE != nullptr;
}

////////////////////////////////////////////////////////////////////////////////

Yast Yast::slice(int begin, int end) const
{
    const int len = static_cast<int>(length());
    if (end < 0)
    {
        end += len + 1;
        if (end < 0)
        {
            end = 0;
        }
    }
    if (end > (len + 1))
    {
        end = len + 1;
    }
    if (begin < 0)
    {
        begin += len;
        if (begin < 0)
        {
            begin = 0;
        }
    }
    if (begin > end)
    {
        begin = end;
    }
    return Yast(m_str + begin, end - begin);
}

////////////////////////////////////////////////////////////////////////////////

int Yast::find(int len, PCWSTR what, int offset) const
{
    if (len == 0)
    {
        return 0;
    }
    const UINT ulen = length();
    const int slen = static_cast<int>(ulen);
    if (ulen > static_cast<UINT>(INT_MAX) || offset >= slen)
    {
        return -1;
    }
    else if (offset < 0)
    {
        offset = 0;
    }
    int result = offset;
    while (result < slen)
    {
        int pos = 0, spos = result;
        while(pos < len && spos < slen && !(*(what + pos) - *(m_str + spos)))
        {
            pos++, spos++;
        }
        if (pos == len)
        {
            return result;
        }
        result++;
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////

int Yast::rfind(PCWSTR what, int len) const
{
    if (len == 0)
    {
        return 0;
    }
    const int slen = static_cast<int>(length());
    int result = slen - len;
    while (result >= 0)
    {
        int pos = 0, spos = result;
        while(pos < len && spos < slen && !(*(what + pos) - *(m_str + spos)))
        {
            pos++, spos++;
        }
        if (pos == len)
        {
            return result;
        }
        result--;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////

Yast& Yast::replace(const Yast& what, const Yast& replacement)
{
    const UINT wlen = what.length();
    if (wlen == 0)
    {
        return *this;
    }

    PCWSTR const swhat = what;
    const UINT len = length();

    cvector<size_t> positions;
    positions.reserve(len / wlen);
    int found_pos = find(wlen, swhat);
    while (found_pos >= 0)
    {
        positions.push_back(found_pos);
        found_pos = find(wlen, swhat, found_pos + wlen);
    }

    const UINT num_rep = static_cast<UINT>(positions.size());
    if (num_rep == 0)
    {
        return *this;
    }

    const UINT rlen = replacement.length();
    UINT new_len = (
        rlen >= wlen ?
        length() + num_rep * (rlen - wlen) :
        length() - num_rep * (wlen - rlen)
        );

    PWSTR const result = allocate(nullptr, new_len);
    PWSTR dest = result;
    PCWSTR const repl = replacement.m_str;
    size_t lead_pos = 0;
    for (auto pos : positions)
    {
        size_t num_lead = pos - lead_pos;
        memcpy(dest, m_str + lead_pos, num_lead * sizeof(WCHAR));
        dest += num_lead;
        memcpy(dest, repl, rlen * sizeof(WCHAR));
        dest += rlen;
        lead_pos = pos + wlen;
    }
    if (lead_pos < len)
    {
        memcpy(dest, m_str + lead_pos, (len - lead_pos) * sizeof(WCHAR));
    }
    release(m_str);
    m_str = result;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

YastVector Yast::split(PCWSTR seperator) const
{
    const int slen = sz_len(seperator);
    YastVector result;
    if (slen == 0)
    {
        result.push_back(*this);
        return result;
    }
    result.reserve(length() / (slen + 1));
    int begin = 0;
    int end = find(slen, seperator);
    while (end >= 0)
    {
        result.push_back(slice(begin, end));
        begin = end + slen;
        end = find(slen, seperator, begin);
    }
    if (begin < static_cast<int>(length()) - 1)
    {
        result.push_back(slice(begin, -1));
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////

Yast& Yast::append(PCWSTR p_str, UINT len)
{
    if (p_str == nullptr || len == 0)
    {
        return *this;
    }

    const UINT this_chars = length();
    const UINT new_chars = this_chars + len;

    YSTR new_str = allocate(nullptr, new_chars);
    memcpy(new_str, m_str, this_chars * sizeof(WCHAR));
    memcpy(new_str + this_chars, p_str, len * sizeof(WCHAR));
    // No need to write a terminating 0, since allocate already wrote it.
    release(m_str);
    m_str = new_str;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

Yast& Yast::expand_env_vars()
{
    // Length including 0.
    const UINT len = ExpandEnvironmentStringsW(m_str, nullptr, 0);
    if (len)
    {
        // Length w/o 0.
        YSTR to_be_deleted = allocate(nullptr, len - 1);
        if (ExpandEnvironmentStringsW(m_str, to_be_deleted, len))
        {
            YSTR tmp = m_str;
            m_str = to_be_deleted;
            to_be_deleted = tmp;
        }
        release(to_be_deleted);
    }
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

Yast& Yast::reverse()
{
    const UINT len = length();
    const UINT half_len = len / 2;
    for (UINT i = 0; i < half_len; i++)
    {
        const UINT eidx = len - i - 1;
        const WCHAR tmp = m_str[i];
        m_str[i] = m_str[eidx];
        m_str[eidx] = tmp;
    }
    return *this;
}

////////////////////////////////////////////////////////////////////////////////

size_t Yast::hash() const
{
    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    constexpr size_t prime = (
        sizeof(size_t) == 8 ? 1099511628211ULL : 16777619U
        );
    constexpr size_t offset = (
        sizeof(size_t) == 8 ? 14695981039346656037ULL : 2166136261U
        );
    const BYTE* const bytes = p2p<const BYTE*>(m_str);
    const UINT blen = byte_length();
    size_t mince = offset;
    for (UINT u = 0; u < blen; u++)
    {
        mince ^= static_cast<size_t>(bytes[u]);
        mince *= prime;
    }
    return mince;
}

////////////////////////////////////////////////////////////////////////////////

bool Yast::binary_same(const Yast& cmp) const
{
    const UINT len = length();
    if (len != cmp.length())
    {
        return false;
    }
    PCWSTR const self = m_str;
    PCWSTR const pcmp = cmp.m_str;
    for (UINT i = 0; i < len; i++)
    {
        if (self[i] != pcmp[i])
        {
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool Yast::to_clipboard(HWND wnd) const
{
    bool done_copy = false;
    // Terminating zero must be part of the transfer.
    const UINT byte_len = byte_length() + sizeof(WCHAR);
    HGLOBAL hcopy = GlobalAlloc(GMEM_MOVEABLE, byte_len);
    if (hcopy)
    {
        void* pcopy = GlobalLock(hcopy);
        if (pcopy)
        {
            memcpy(pcopy, m_str, byte_len);
            GlobalUnlock(hcopy);
            if (OpenClipboard(wnd) && EmptyClipboard())
            {
                done_copy = (0 != SetClipboardData(CF_UNICODETEXT, hcopy));
                CloseClipboard();
            }
        }
        if (!done_copy)
        {
            GlobalFree(hcopy);
        }
    }
    return done_copy;
}

////////////////////////////////////////////////////////////////////////////////

bool Yast::from_clipboard(HWND wnd)
{
    bool result = false;
    if (IsClipboardFormatAvailable(CF_UNICODETEXT) && OpenClipboard(wnd))
    {
        HGLOBAL hmem = GetClipboardData(CF_UNICODETEXT);
        if (hmem != nullptr)
        {
            PWSTR cbs = static_cast<PWSTR>(GlobalLock(hmem));
            if (cbs != nullptr)
            {
                release(m_str);
                m_str = allocate(cbs, sz_len(cbs));
                GlobalUnlock(hmem);
                result = true;
            }
        }
        CloseClipboard();
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////
