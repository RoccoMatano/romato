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

#pragma once

#include "list_ctrl_pdata.h"

struct ItemProgress : public LcpdItemData
{
    short PercentColumn;
    unsigned short ProgressPercent;
    bool ShowProgress;

    ItemProgress() :
        PercentColumn(0),
        ProgressPercent(0),
        ShowProgress(false)
    {
    }
};


class ListCtrlProgress : public ListCtrlPData<ItemProgress>
{
public:
    ListCtrlProgress(HWND hWnd = nullptr) : ListCtrlPData(hWnd)
    {
    }
    ListCtrlProgress& operator=(const BaseWnd& src)
    {
        ListCtrlPData::operator=(src);
        return *this;
    }

    // Parent can feed WM_NOTIFY messages from this control into
    // this method. Doing so is mandatory, if this control is meant to
    // paint progress bars.
    LRESULT HandleReflectedNotify(NMHDR* pNMHDR);

    void SetProgress(int idx, int col_idx, int percent = 0);

protected:

    LRESULT OnCustomDraw(NMLVCUSTOMDRAW* pLVCD);
};
