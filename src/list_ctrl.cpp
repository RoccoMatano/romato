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

#include "romato.h"
#include "list_ctrl.h"

////////////////////////////////////////////////////////////////////////////////

CRect ListCtrl::GetItemRect(int idx)
{
    return GetSubItemRect(idx, -1);
}

////////////////////////////////////////////////////////////////////////////////

CRect ListCtrl::GetSubItemRect(int idx, int col_idx)
{
    CRect rc = {LVIR_BOUNDS, col_idx < 1 ? 0 : col_idx, 0, 0};
    SendMessage(LVM_GETSUBITEMRECT, idx, p2lp(&rc));
    if (col_idx == 0 && GetColumnCount() > 1)
    {
        CRect tmp = {LVIR_BOUNDS, 1, 0, 0};
        SendMessage(LVM_GETSUBITEMRECT, idx, p2lp(&tmp));
        rc.right = tmp.left;
    }
    return rc;
}

////////////////////////////////////////////////////////////////////////////////

void ListCtrl::DeleteAllItems()
{
    SendMessage(LVM_DELETEALLITEMS, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////

bool ListCtrl::DeleteItem(int idx)
{
    return 0 != SendMessage(LVM_DELETEITEM, idx, 0);
}

////////////////////////////////////////////////////////////////////////////////

int ListCtrl::FindItem(LVFINDINFO& info, int start)
{
    return static_cast<int>(
        SendMessage(LVM_FINDITEM, start, p2lp(&info))
        );
}

////////////////////////////////////////////////////////////////////////////////

int ListCtrl::GetColumnCount()
{
    BaseWnd hdr(i2p<HWND>(SendMessage(LVM_GETHEADER, 0, 0)));
    return static_cast<int>(hdr.SendMessage(HDM_GETITEMCOUNT, 0, 0));
}

////////////////////////////////////////////////////////////////////////////////

bool ListCtrl::GetItem(LVITEM& item)
{
    return 0 != SendMessage(LVM_GETITEM, 0, p2lp(&item));
}

////////////////////////////////////////////////////////////////////////////////

int ListCtrl::GetItemCount()
{
    return static_cast<int>(SendMessage(LVM_GETITEMCOUNT, 0, 0));
}

////////////////////////////////////////////////////////////////////////////////

intptr_t ListCtrl::GetItemData(int idx)
{
    LVITEM item;
    item.mask = LVIF_PARAM;
    item.iItem = idx;
    item.lParam = 0;
    SendMessage(LVM_GETITEM, 0, p2lp(&item));
    return item.lParam;
}

////////////////////////////////////////////////////////////////////////////////

int ListCtrl::GetNextItem(int idx, int flags)
{
    return static_cast<int>(SendMessage(LVM_GETNEXTITEM, idx, flags));
}

////////////////////////////////////////////////////////////////////////////////

UINT ListCtrl::GetSelectedCount()
{
    return static_cast<UINT>(SendMessage(LVM_GETSELECTEDCOUNT, 0, 0));
}

////////////////////////////////////////////////////////////////////////////////

int ListCtrl::GetSelectionMark()
{
    return static_cast<int>(SendMessage(LVM_GETSELECTIONMARK, 0, 0));
}

////////////////////////////////////////////////////////////////////////////////

int ListCtrl::InsertColumn(int idx, LVCOLUMN& column)
{
    return static_cast<int>(
        SendMessage(LVM_INSERTCOLUMN, idx, p2lp(&column))
        );
}

////////////////////////////////////////////////////////////////////////////////

int ListCtrl::InsertItem(LVITEM& item)
{
    return static_cast<int>(
        SendMessage(LVM_INSERTITEM, 0, p2lp(&item))
        );
}

////////////////////////////////////////////////////////////////////////////////

bool ListCtrl::SetColumnWidth(int idx, int width)
{
    return 0 != SendMessage(LVM_SETCOLUMNWIDTH, idx, width);
}

////////////////////////////////////////////////////////////////////////////////

bool ListCtrl::SetItemData(int idx, intptr_t data)
{
    LVITEM item;
    item.mask = LVIF_PARAM;
    item.iItem = idx;
    item.lParam = data;
    return 0 != SendMessage(LVM_SETITEM, 0, p2lp(&item));
}

////////////////////////////////////////////////////////////////////////////////

bool ListCtrl::SetItemState(int idx, UINT state, UINT mask)
{
    LVITEM item;
    item.stateMask = mask;
    item.state = state;
    return 0 != SendMessage(LVM_SETITEMSTATE, idx, p2lp(&item));
}

////////////////////////////////////////////////////////////////////////////////

bool ListCtrl::SetItemText(int idx, int col_idx, PCTSTR pText)
{
    LVITEM item;
    item.iSubItem = col_idx;
    item.pszText = const_cast<PTSTR>(pText);
    return 0 != SendMessage(LVM_SETITEMTEXT, idx, p2lp(&item));
}

////////////////////////////////////////////////////////////////////////////////

void ListCtrl::SetImageList(HIMAGELIST img_lst, int type)
{
    SendMessage(LVM_SETIMAGELIST, type, p2lp(img_lst));
}

////////////////////////////////////////////////////////////////////////////////

void ListCtrl::SetStyle(UINT style)
{
    SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, style, style);
}

////////////////////////////////////////////////////////////////////////////////

Yast ListCtrl::GetItemText(int idx, int col_idx)
{
    Yast tmp;
    LVITEM item;
    UINT rlen, len = 128;
    item.iSubItem = col_idx;
    do
    {
        len *= 2;
        tmp.clear(len);
        item.cchTextMax = len + 1;
        item.pszText = tmp;
        rlen = static_cast<UINT>(
            SendMessage(LVM_GETITEMTEXT, idx, p2lp(&item))
            );
    }
    while (rlen >= len);
    return tmp.adapt_to_sz();
}

////////////////////////////////////////////////////////////////////////////////
