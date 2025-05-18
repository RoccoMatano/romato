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

struct StringResourceEntry
{
    WORD m_length;
    WCHAR m_string[ANYSIZE_ARRAY];
};

inline StringResourceEntry* FindStringResource(
    HINSTANCE h_inst,
    UINT str_id,
    UINT lang_id
    )
{
    // This code deliberately ignores 16 bit semantics for resources.
    // I.e. there is no call to LockResource, UnlockResource or FreeResource.

    WORD* p_tmp = static_cast<WORD*>(
        LoadResource(
            h_inst,
            FindResourceEx(
                h_inst,
                RT_STRING,
                MAKEINTRESOURCE(str_id / 16 + 1),
                static_cast<WORD>(lang_id)
                )
            )
        );
    if (p_tmp != nullptr)
    {
        for (UINT i = 0; i < (str_id & 15); i++)
        {
            p_tmp += (1 + *p_tmp);
        }
    }
    return reinterpret_cast<StringResourceEntry*>(p_tmp);
}
