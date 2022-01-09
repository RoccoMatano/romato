////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the romato library.
//
// Copyright 2013-2022 Rocco Matano
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

// simple wrapper for LVM_* messages

class ListCtrl : public BaseWnd
{
public:
    ListCtrl(HWND hWnd = nullptr) : BaseWnd(hWnd)
    {
    }
    ListCtrl& operator=(const BaseWnd& src)
    {
        m_hWnd = src.m_hWnd;
        return *this;
    }


    int      GetItemCount();
    bool     GetItem(LVITEM& item);
    intptr_t GetItemData(int idx);
    int      GetNextItem(int idx, int flags);
    UINT     GetSelectedCount();
    int      GetSelectionMark();
    Yast     GetItemText(int idx, int col_idx);
    CRect    GetItemRect(int idx);
    CRect    GetSubItemRect(int idx, int col_idx);

    int      GetColumnCount();
    int      InsertColumn(int idx, LVCOLUMN& column);

    int      InsertItem(LVITEM& item);
    int      FindItem(LVFINDINFO& info, int start = -1);

    void     DeleteAllItems();
    bool     DeleteItem(int idx);

    bool     SetColumnWidth(int idx, int width);
    bool     SetItemData(int idx, intptr_t data);
    bool     SetItemState(int idx, UINT state, UINT mask);
    bool     SetItemText(int idx, int col_idx, PCTSTR pText);
    void     SetImageList(HIMAGELIST img_lst, int type);
    void     SetStyle(UINT style);

};
