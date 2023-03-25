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

#if defined(UNICODE)

#include "romato_macros.h"
#include "yast.h"

////////////////////////////////////////////////////////////////////////////////

ALWAYSINLINE bool WriteRegString(HKEY Key, PCWSTR Name, const Yast& Src)
{
    return (
        ERROR_SUCCESS == RegSetValueEx(
            Key,
            Name,
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(static_cast<PCWSTR>(Src)),
            Src.byte_length()
            )
        );
}

////////////////////////////////////////////////////////////////////////////////

ALWAYSINLINE bool WriteRegDword(HKEY Key, PCWSTR Name, DWORD Src)
{
    return (
        ERROR_SUCCESS == RegSetValueEx(
            Key,
            Name,
            0,
            REG_DWORD,
            reinterpret_cast<const BYTE*>(&Src),
            sizeof(DWORD)
            )
        );
}

////////////////////////////////////////////////////////////////////////////////

ALWAYSINLINE bool WriteRegBinary(
    HKEY Key,
    PCWSTR Name,
    const void* source,
    UINT length
    )
{
    return (
        ERROR_SUCCESS == RegSetValueEx(
            Key,
            Name,
            0,
            REG_BINARY,
            reinterpret_cast<const BYTE*>(source),
            length
            )
        );
}

////////////////////////////////////////////////////////////////////////////////

ALWAYSINLINE LONG CreateKey(HKEY Key, PCWSTR SubKey, PHKEY Result)
{
    const REGSAM acc = KEY_READ | DELETE | KEY_WRITE;
    return RegCreateKeyEx(Key, SubKey, 0, 0, 0, acc, 0, Result, 0);
}

////////////////////////////////////////////////////////////////////////////////

ALWAYSINLINE LONG EnumKeys(HKEY hKey, DWORD idx, PWSTR Name, DWORD* NameLen)
{
    return RegEnumKeyEx(hKey, idx, Name, NameLen, 0, 0, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////

ALWAYSINLINE LONG EnumValueNames(
    HKEY hKey,
    DWORD idx,
    PWSTR Name,
    DWORD* NameLen
    )
{
    return RegEnumValue(hKey, idx, Name, NameLen, 0, 0, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////

inline bool ReadRegDword(HKEY Key, PCWSTR Name, DWORD& Dest)
{
    DWORD rType = ~0U;
    LONG s = RegQueryValueEx(Key, Name, nullptr, &rType, nullptr, nullptr);
    if (s != ERROR_SUCCESS || rType != REG_DWORD)
    {
        return false;
    }
    DWORD n = sizeof(DWORD);
    return (
        ERROR_SUCCESS == RegQueryValueEx(
            Key,
            Name,
            nullptr,
            &rType,
            reinterpret_cast<PBYTE>(&Dest),
            &n
            )
        );
}

inline bool ReadRegDword(HKEY Key, PCWSTR Name, UINT& Dest)
{
    DWORD tmp;
    if (ReadRegDword(Key, Name, tmp))
    {
        Dest = tmp;
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

inline bool ReadRegBool(HKEY Key, PCWSTR Name, bool& Dest)
{
    DWORD tmp;
    if (ReadRegDword(Key, Name, tmp))
    {
        Dest = (tmp != 0);
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////

inline bool ReadRegString(HKEY Key, PCWSTR Name, Yast& Dest)
{
    DWORD rType = ~0U, NumBytes = 0;
    LONG s = RegQueryValueEx(Key, Name, nullptr, &rType, nullptr, &NumBytes);
    if (s != ERROR_SUCCESS || rType != REG_SZ || (NumBytes & 1))
    {
        return false;
    }
    UINT NumChars = NumBytes / sizeof(WCHAR);
    Dest.clear(NumChars);
    return (
        ERROR_SUCCESS == RegQueryValueEx(
            Key,
            Name,
            nullptr,
            &rType,
            reinterpret_cast<PBYTE>(static_cast<PWSTR>(Dest)),
            &NumBytes
            )
        );
}

////////////////////////////////////////////////////////////////////////////////

inline bool ReadRegBinary(HKEY Key, PCWSTR Name, void* dest, UINT length)
{
    DWORD rType = ~0U, NumBytes = 0;
    LONG s = RegQueryValueEx(Key, Name, nullptr, &rType, nullptr, &NumBytes);
    if (s != ERROR_SUCCESS || rType != REG_BINARY || (NumBytes != length))
    {
        return false;
    }
    return (
        ERROR_SUCCESS == RegQueryValueEx(
            Key,
            Name,
            nullptr,
            &rType,
            reinterpret_cast<PBYTE>(dest),
            &NumBytes
            )
        );
}


////////////////////////////////////////////////////////////////////////////////

class RegAutoKey
{
private:
    HKEY m_key;

public:
    RegAutoKey(PCWSTR SubKey, HKEY Key = nullptr)
    {
        const REGSAM acc = KEY_READ | DELETE | KEY_WRITE;
        HKEY tmp = nullptr;
        if (Key == nullptr)
        {
            RegCreateKeyExW(
                HKEY_CURRENT_USER,
                L"Software",
                0,
                nullptr,
                0,
                acc,
                nullptr,
                &tmp,
                nullptr
                );
            Key = tmp;
        }
        RegCreateKeyExW(
            Key,
            SubKey,
            0,
            nullptr,
            0,
            acc,
            nullptr,
            &m_key,
            nullptr
            );
        if (tmp)
        {
            RegCloseKey(tmp);
        }
    }
    operator HKEY() const
    {
        return m_key;
    }
    bool operator !()  const
    {
        return m_key == nullptr;
    }
    ~RegAutoKey()
    {
        RegCloseKey(m_key);
    }
};

////////////////////////////////////////////////////////////////////////////////

#endif // defined(UNICODE)
