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

#include "base_wnd.h"

////////////////////////////////////////////////////////////////////////////////

struct WND_CREATE : public WNDCLASSW
{
//  members of WNDCLASSW
//  UINT        style;
//  WNDPROC     lpfnWndProc;
//  int         cbClsExtra;
//  int         cbWndExtra;
//  HINSTANCE   hInstance;
//  HICON       hIcon;
//  HCURSOR     hCursor;
//  HBRUSH      hbrBackground;
//  LPCWSTR     lpszMenuName;
//  LPCWSTR     lpszClassName;
    BaseWnd*    Parent;
    PCWSTR      WndName;
    HMENU       WndMenu;
    DWORD       WndStyle;
    DWORD       ExStyle;
    int         Left;
    int         Top;
    int         Width;
    int         Height;

    WND_CREATE()
    {
        // hCursor == nullptr will be replaced by IDC_ARROW in SimpleWnd::Create
        memset(this, 0, sizeof(*this));
    }
};

////////////////////////////////////////////////////////////////////////////////

// Since this class is only meant for inheritance and not for instantiation,
// save same bytes by omitting the vtable

class NO_VTABLE SimpleWnd : public BaseWnd
{
public:

    void Create(WND_CREATE& CreateParams);

    ////////////////////////////////////////////////////////////////////////////

protected:

    BaseWnd *m_Parent;

    ////////////////////////////////////////////////////////////////////////////

    virtual LRESULT OnMessage(UINT msg, WPARAM wp, LPARAM lp) = 0;

    ////////////////////////////////////////////////////////////////////////////

    SimpleWnd() : BaseWnd(), m_Parent(nullptr)
    {
    }

    ////////////////////////////////////////////////////////////////////////////

    virtual ~SimpleWnd()
    {
        // In WndProc we set m_hWnd to nullptr when WM_NCDESTROY is processed.
        // So either WM_NCDESTROY was processed (i.e. this window already has
        // been destroyed and the following DestroyWindow will operate on a
        // window handle of nullptr having no effect) or this window still needs
        // to be destroyed, before we can allow the memory for this object to
        // vanish.
        Destroy();
    }

    ////////////////////////////////////////////////////////////////////////////

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

    ////////////////////////////////////////////////////////////////////////////

private:
    SimpleWnd(const SimpleWnd& src) = delete;
    SimpleWnd& operator=(SimpleWnd& src) = delete;
};

////////////////////////////////////////////////////////////////////////////////
