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
#include "list_ctrl_drop_files.h"

////////////////////////////////////////////////////////////////////////////////

LRESULT ListCtrlDropFiles::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    ListCtrlDropFiles* self = i2p<ListCtrlDropFiles*>(
        GetWindowLongPtrW(hWnd, GWLP_USERDATA)
        );
    if (self != nullptr)
    {
        if (msg == WM_DROPFILES && self->m_pCallback)
        {
            HDROP const drop_info = i2p<HDROP>(wp);
            Yast filename;
            const UINT cnt = DragQueryFileW(drop_info, ~0U, nullptr, 0);
            for (UINT n = 0 ; n < cnt; n++)
            {
                UINT len = DragQueryFileW(drop_info, n, nullptr, 0);
                filename.clear(len);
                DragQueryFileW(drop_info, n, filename, len + 1);
                self->m_pCallback(self->m_pCtxt, filename);
            }
            return 0;
        }
        return CallWindowProc(self->m_original_wnd_proc, hWnd, msg, wp, lp);
    }
    return DefWindowProc(hWnd, msg, wp, lp);
}

////////////////////////////////////////////////////////////////////////////////

bool ListCtrlDropFiles::Attach(
    BaseWnd lst_wnd,
    LIST_CTRL_DROP_FILES_CALLBACK pCallback,
    void *pCtxt
    )
{
    if (
        m_hWnd != nullptr ||
        m_original_wnd_proc != nullptr ||
        !lst_wnd.IsWindow()
        )
    {
        return false;
    }

    m_hWnd = lst_wnd.m_hWnd;
    m_pCallback = pCallback;
    m_pCtxt = pCtxt;
    SetWindowLongPtr(m_hWnd, GWLP_USERDATA, p2i<LONG_PTR>(this));
    void *pfunc = WndProc; // remove ambiguity about constness
    m_original_wnd_proc = i2p<WNDPROC>(
        SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, p2i<LONG_PTR>(pfunc))
        );
    DragAcceptFiles(m_hWnd, true);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
