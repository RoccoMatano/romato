////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the romato library.
//
// Copyright 2013-2021 Rocco Matano
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

////////////////////////////////////////////////////////////////////////////////
//
// A list control with private item data.
// The type of the private data is given by <class T>, which has to be derived
// from LcpdItemData. For every item a T object is allocated an the heap by
// calling 'new T()'.
//
// Since this control creates a heap object for every list item, care must be
// taken to ensure that all those objects are deleted. See operator= and
// OnDestroy.
//
////////////////////////////////////////////////////////////////////////////////

struct LcpdItemData
{
    intptr_t user_data;

    LcpdItemData() : user_data(0)
    {
    }
};

////////////////////////////////////////////////////////////////////////////////

template <class T> class ListCtrlPData : public ListCtrl
{
public:

    static_assert(
        std::is_base_of<LcpdItemData, T>::value,
        "T does not inherit from LcpdItemData"
        );

    ListCtrlPData(HWND hWnd = nullptr) : ListCtrl(hWnd)
    {
    }
    ListCtrlPData& operator=(const BaseWnd& src)
    {
        DeleteAllItems();
        m_hWnd = src.m_hWnd;
        return *this;
    }

    // handler for WM_DESTROY
    void OnDestroy()
    {
        DeleteAllItems();
    }

    // need to override all methods that might have to deal with item data

    bool DeleteItem(int idx);
    void DeleteAllItems();

    int InsertItem(LVITEM& item);
    bool GetItem(LVITEM& item);

    bool SetItemData(int idx, intptr_t data);
    intptr_t GetItemData(int idx);

protected:
    T* GetPrivateItemData(int idx);
};

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


