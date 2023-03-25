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
#include "simple_wnd.h"

////////////////////////////////////////////////////////////////////////////////

LRESULT SimpleWnd::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg != WM_NCCREATE)
    {
        SimpleWnd* self = i2p<SimpleWnd*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (self)
        {
            const LRESULT res = self->OnMessage(msg, wp, lp);
            if (msg == WM_NCDESTROY)
            {
                self->m_hWnd = nullptr;
            }
            return res;
        }
        else
        {
            // Other messages than WM_NCCREATE might arrive before WM_NCCREATE
            // (e.g. WM_GETMINMAXINFO), i.e. still self == nullptr.
            TRACE("no self for msg %04x, wnd %p\n", msg, hWnd);
            return DefWindowProc(hWnd, msg, wp, lp);
        }
    }

    // Since we control window creation (see below), where we supply 'this' as
    // lpCreateParams, we don't have to test self for nullptr.
    SimpleWnd* self = p2p<SimpleWnd*>(i2p<CREATESTRUCT*>(lp)->lpCreateParams);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, p2i<LONG_PTR>(self));
    self->m_hWnd = hWnd;
    return self->OnMessage(msg, wp, lp);
}

////////////////////////////////////////////////////////////////////////////////

void SimpleWnd::Create(WND_CREATE& wc)
{
    m_Parent = wc.Parent;

    wc.lpfnWndProc = WndProc;
    if (!wc.hInstance)
    {
        wc.hInstance = GetCallerHinstance();
    }
    if (!wc.hCursor)
    {
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    }

    WCHAR cname[16];
    if (!wc.lpszClassName)
    {
        // calc Fowler-Noll-Vo hash over WNDCLASSW members only
        const BYTE* const bytes = p2p<const BYTE*>(&wc);
        uint32_t hash = 2166136261U;
        for (UINT u = 0; u < sizeof(WNDCLASSW); u++)
        {
            hash ^= static_cast<uint32_t>(bytes[u]);
            hash *= 16777619U;
        }
        wsprintf(cname, L"rml:%x", hash);
        wc.lpszClassName = cname;
    }

    if (!GetClassInfo(wc.hInstance, wc.lpszClassName, &wc))
    {
        if (!RegisterClass(&wc))
        {
            goto ErrorExit;
        }
    }
    if (
        nullptr == CreateWindowEx(
            wc.ExStyle,
            wc.lpszClassName,
            wc.WndName,
            wc.WndStyle,
            wc.Left,
            wc.Top,
            wc.Width,
            wc.Height,
            wc.Parent ? wc.Parent->m_hWnd : nullptr,
            wc.WndMenu,
            wc.hInstance,
            this
            )
        )
    {
        goto ErrorExit;
    }
    return;

ErrorExit:
    RaiseException(HRESULT_FROM_WIN32(GetLastError()));
}

////////////////////////////////////////////////////////////////////////////////
