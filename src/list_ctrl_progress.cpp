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
#include "list_ctrl_progress.h"

////////////////////////////////////////////////////////////////////////////////

LRESULT ListCtrlProgress::HandleReflectedNotify(NMHDR* pNMHDR)
{
    if (pNMHDR->hwndFrom != m_hWnd || pNMHDR->code != NM_CUSTOMDRAW)
    {
        return 0;
    }
    NMLVCUSTOMDRAW* pLVCD = p2p<NMLVCUSTOMDRAW*>(pNMHDR);
    return OnCustomDraw(pLVCD);
}

////////////////////////////////////////////////////////////////////////////////

void ListCtrlProgress::SetProgress(int idx, int col_idx, int percent)
{
    ItemProgress* pid = GetPrivateItemData(idx);
    if (!pid)
    {
        return;
    }
    const unsigned short spercent = (
        percent < 0 ? 0 : (
            percent > 100 ?
            100 :
            static_cast<unsigned short>(percent)
            )
        );

    bool changed = !pid->ShowProgress;
    pid->ShowProgress = true;
    if (pid->PercentColumn != col_idx)
    {
        pid->PercentColumn = static_cast<short>(col_idx);
        changed = true;
    }
    if (pid->ProgressPercent != spercent)
    {
        pid->ProgressPercent = spercent;
        changed = true;
    }

    if (changed)
    {
        CRect rect(GetSubItemRect(idx, col_idx));
        InvalidateRect(&rect, true);
        Update();
    }
}

////////////////////////////////////////////////////////////////////////////////

LRESULT ListCtrlProgress::OnCustomDraw(NMLVCUSTOMDRAW* pLVCD)
{
    // First we have to check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
    if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        return CDRF_NOTIFYITEMDRAW;
    }

    if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        // This is the notification message for an item.  We'll request
        // notifications before each subitem's prepaint stage.
        return CDRF_NOTIFYSUBITEMDRAW;
    }

    if (pLVCD->nmcd.dwDrawStage != (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
    {
        // This is NOT the prepaint stage for a subitem.
        return CDRF_DODEFAULT;
    }

    // This IS the prepaint stage for a subitem.

    int idx = static_cast<int>(pLVCD->nmcd.dwItemSpec);
    int col = pLVCD->iSubItem;

    ItemProgress* pid = i2p<ItemProgress*>(pLVCD->nmcd.lItemlParam);
    if (!pid || pid->PercentColumn != col || !pid->ShowProgress)
    {
        return CDRF_DODEFAULT;
    }

    CRect rect(GetSubItemRect(idx, col));
    HDC hdc = pLVCD->nmcd.hdc;

    rect.bottom -= 1;
    rect.left += 1;     // leave margin in case row is highlighted

    // fill interior with light gray
    FillSolidRect(hdc, &rect, RGB(224,224,224));

    // draw border
    RECT tmp;
    tmp.left = rect.left;
    tmp.top = rect.top;
    tmp.right = rect.right - 1;
    tmp.bottom = rect.top + 1;
    FillSolidRect(hdc, &tmp, RGB(0,0,0));
    tmp.right = rect.left + 1;
    tmp.bottom = rect.bottom - 1;
    FillSolidRect(hdc, &tmp, RGB(0,0,0));
    tmp.left = rect.right;
    tmp.right = rect.right - 1;
    tmp.bottom = rect.bottom;
    FillSolidRect(hdc, &tmp, GetSysColor(COLOR_BTNSHADOW));
    tmp.left = rect.left;
    tmp.top = rect.bottom;
    tmp.right = rect.right;
    tmp.bottom = rect.bottom - 1;
    FillSolidRect(hdc, &tmp, GetSysColor(COLOR_BTNSHADOW));

    const int percent = pid->ProgressPercent;
    if (percent > 0)
    {
        TCHAR txt[8];
        SZ_PRINTF(txt, TEXT("%d%%"), percent);
        RECT left, right, trc;
        left.left = rect.left + 1;
        left.top = rect.top + 1;
        left.right = rect.right;
        left.bottom = rect.bottom - 1;
        right.top = left.top;
        right.right = left.right;
        right.bottom = left.bottom;
        int width = (left.right - left.left) * percent / 100;
        left.right = left.left + width - 1;
        right.left = left.right;
        FillSolidRect(hdc, &left, GetSysColor(COLOR_HIGHLIGHT));
        SetBkMode(hdc, TRANSPARENT);
        trc.left = rect.left + 1;
        trc.top = rect.top + 1;
        trc.right = rect.right - 1;
        trc.bottom = rect.bottom - 1;

        HRGN rgn = CreateRectRgn(left.left, left.top, left.right, left.bottom);
        SelectClipRgn(hdc, rgn);
        SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
        DrawText(hdc, txt, -1, &trc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        DeleteObject(rgn);

        rgn = CreateRectRgn(right.left, right.top, right.right, right.bottom);
        SelectClipRgn(hdc, rgn);
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
        DrawText(hdc, txt, -1, &trc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        DeleteObject(rgn);
        SelectClipRgn(hdc, nullptr);
    }
    // We've painted everything.
    return CDRF_SKIPDEFAULT;
}

////////////////////////////////////////////////////////////////////////////////
