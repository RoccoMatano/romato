////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the romato library.
//
// Copyright 2013-2026 Rocco Matano
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
#include "list_ctrl_pdata.h"

////////////////////////////////////////////////////////////////////////////////

template <class T> T* ListCtrlPData<T>::GetPrivateItemData(int idx)
{
    LVITEM lvi;
    lvi.mask = LVIF_PARAM;
    lvi.iItem = idx;
    lvi.lParam = 0;
    SendMessage(LVM_GETITEM, 0, p2lp(&lvi));
    return i2p<T*>(lvi.lParam);
}

////////////////////////////////////////////////////////////////////////////////

template <class T> bool ListCtrlPData<T>::DeleteItem(int idx)
{
    delete GetPrivateItemData(idx);
    return 0 != SendMessage(LVM_DELETEITEM, idx, 0);
}

////////////////////////////////////////////////////////////////////////////////

template <class T> void ListCtrlPData<T>::DeleteAllItems()
{
    const int cnt = GetItemCount();
    for (int i = 0; i < cnt; i++)
    {
        delete GetPrivateItemData(i);
    }
    SendMessage(LVM_DELETEALLITEMS, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////

template <class T> bool ListCtrlPData<T>::GetItem(LVITEM& item)
{
    bool res = (0 != SendMessage(LVM_GETITEM, 0, p2lp(&item)));
    T* pid = i2p<T*>(item.lParam);
    if (pid)
    {
        item.lParam = pid->user_data;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////

template <class T> int ListCtrlPData<T>::InsertItem(LVITEM& item)
{
    const int idx = static_cast<int>(
        SendMessage(LVM_INSERTITEM, 0, p2i<LPARAM>(&item))
        );
    if (idx >= 0)
    {
        T* pid = new T();
        pid->user_data = item.lParam;

        LVITEM lvi;
        lvi.iItem = idx;
        lvi.iSubItem = 0;
        lvi.mask = LVIF_PARAM;
        lvi.lParam = p2lp(pid);
        SendMessage(LVM_SETITEM, 0, p2lp(&lvi));
    }
    return idx;
}

////////////////////////////////////////////////////////////////////////////////

template <class T> bool ListCtrlPData<T>::SetItemData(int idx, intptr_t data)
{
    T* pid = GetPrivateItemData(idx);
    if (pid)
    {
        pid->user_data = data;
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

template <class T> intptr_t ListCtrlPData<T>::GetItemData(int idx)
{
    T* pid = GetPrivateItemData(idx);
    return pid ? pid->user_data : 0;
}

////////////////////////////////////////////////////////////////////////////////

