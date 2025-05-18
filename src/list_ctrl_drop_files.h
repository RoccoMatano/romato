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

#include "list_ctrl.h"

// a list control that accepts dropping files

typedef void (*LIST_CTRL_DROP_FILES_CALLBACK)(void *pCtxt, const Yast& filename);

class ListCtrlDropFiles : public ListCtrl
{
public:
    ListCtrlDropFiles() :
        ListCtrl(nullptr),
        m_original_wnd_proc(nullptr),
        m_pCallback(nullptr),
        m_pCtxt(nullptr)
    {
    }

    ListCtrlDropFiles(const ListCtrlDropFiles& src) = delete;
    ListCtrlDropFiles& operator=(ListCtrlDropFiles& src) = delete;

    bool Attach(
        BaseWnd lst_wnd,
        LIST_CTRL_DROP_FILES_CALLBACK pCallback = nullptr,
        void *pCtxt = nullptr
        );

protected:

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
    void OnDropFiles(HDROP drop_info);

    WNDPROC m_original_wnd_proc;
    LIST_CTRL_DROP_FILES_CALLBACK m_pCallback;
    void *m_pCtxt;

};
