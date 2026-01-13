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
#include "base_dlg.h"

////////////////////////////////////////////////////////////////////////////////

INT_PTR BaseDlg::DlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg != WM_INITDIALOG)
    {
        auto self = i2p<BaseDlg*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
        if (self != nullptr)
        {
            if (msg == WM_COMMAND)
            {
                return self->OnCommand(LOWORD(wp), HIWORD(wp), i2p<HWND>(lp));
            }
            else if (msg == WM_NOTIFY)
            {
                return self->OnNotify(static_cast<UINT>(wp), i2p<NMHDR*>(lp));
            }
            else
            {
                if (msg == WM_ACTIVATE && self->m_pParent)
                {
                    NM_MSD_ACTIVATE ma = {
                        {hWnd, 0, MSDN_ACTIVATE}, wp != WA_INACTIVE
                        };
                    self->m_pParent->SendNotify(&ma.hdr);
                }
                const auto res = self->OnMessage(msg, wp, lp);
                if (msg == WM_NCDESTROY)
                {
                    self->m_hWnd = nullptr;
                }
                return res;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        auto self = i2p<BaseDlg*>(lp);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, p2i<LONG_PTR>(self));
        self->m_hWnd = hWnd;
        return self->OnInitDialog();
    }
}

////////////////////////////////////////////////////////////////////////////////
