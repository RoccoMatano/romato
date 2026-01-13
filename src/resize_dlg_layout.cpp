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
#include "resize_dlg_layout.h"

////////////////////////////////////////////////////////////////////////////////

void ResizeDlgLayout::Initialize(BaseWnd dlg, int min_width, int min_height)
{
    m_dlg = dlg;
    if ((min_width == 0) || (min_height == 0))
    {
        CRect rc(m_dlg.WindowRect());
        m_min_parent_dims.x = rc.Width();
        m_min_parent_dims.y = rc.Height();
    }
    if (min_width  != 0) m_min_parent_dims.x = min_width;
    if (min_height != 0) m_min_parent_dims.y = min_height;
}

////////////////////////////////////////////////////////////////////////////////

static inline bool is_group_box(BaseWnd& wnd)
{
    if (BS_GROUPBOX == (wnd.GetStyle() & BS_GROUPBOX))
    {
        WCHAR classname[32];
        GetClassName(wnd.m_hWnd, classname, sizeof(classname));
        return 0 == sz_cmp(classname, WC_BUTTON);
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

bool ResizeDlgLayout::AnchorControl(
    AnchorPoint upper_left,
    AnchorPoint lower_right,
    int id,
    bool redraw
    )
{
    BaseWnd item = m_dlg.GetDlgItem(id);
    if (item.m_hWnd == nullptr)
    {
        return false;
    }

    CtrlInfo ci;
    ci.upper_left = upper_left;
    ci.lower_right = lower_right;
    ci.id = id;

    // Group boxes overlap with other controls and therefore do not paint in
    // their inner region. That means that their area always has to be
    // repainted (the background has to be erased). So if want less flicker,
    // you have to avoid group boxes.
    ci.redraw = redraw || is_group_box(item);

    CRect rc_ctrl(item.WindowRectAsOtherClient(m_dlg));
    CRect rc_dlg(m_dlg.ClientRect());

    POINT pt;
    ap2pt(upper_left, rc_dlg.right, rc_dlg.bottom, &pt);
    ci.ul_delta.x = pt.x - rc_ctrl.left;
    ci.ul_delta.y = pt.y - rc_ctrl.top;

    ap2pt(lower_right, rc_dlg.right, rc_dlg.bottom, &pt);
    ci.lr_delta.x = pt.x - rc_ctrl.right;
    ci.lr_delta.y = pt.y - rc_ctrl.bottom;

    m_ctrl_data.push_back(ci);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool ResizeDlgLayout::AnchorControls(const CtrlAnchor *anchors, int num_anchors)
{
    bool ok = true;
    for (int i = 0; ok && i < num_anchors; i++)
    {
        const CtrlAnchor& a = anchors[i];
        ok = AnchorControl(a.upper_left, a.lower_right, a.id, a.redraw);
    }
    return ok;
}

////////////////////////////////////////////////////////////////////////////////

void ResizeDlgLayout::OnSize(int cx, int cy)
{
    // Create region consisting of all areas occupied by controls
    HRGN hrgnPaint = CreateRectRgn(0, 0, 0, 0);
    for (const CtrlInfo& ci : m_ctrl_data)
    {
        BaseWnd item = m_dlg.GetDlgItem(ci.id);
        CRect rc_ctrl(item.WindowRectAsOtherClient(m_dlg));

        HRGN hrgnTemp = CreateRectRgnIndirect(&rc_ctrl);
        CombineRgn(hrgnPaint, hrgnPaint, hrgnTemp, RGN_OR);
        DeleteObject(hrgnTemp);
    }

    for (const CtrlInfo& ci : m_ctrl_data)
    {
        // get control's new upper left corner
        RECT rc_ctrl;
        ap2pt(ci.upper_left, cx, cy, p2p<PPOINT>(&rc_ctrl));
        rc_ctrl.left -= ci.ul_delta.x;
        rc_ctrl.top  -= ci.ul_delta.y;

        // get control's new lower right corner
        ap2pt(ci.lower_right, cx, cy, p2p<PPOINT>(&rc_ctrl.right));
        rc_ctrl.right  -= ci.lr_delta.x;
        rc_ctrl.bottom -= ci.lr_delta.y;

        // position/size the control
        BaseWnd item = m_dlg.GetDlgItem(ci.id);
        item.Move(rc_ctrl, false);
        if (ci.redraw)
        {
            item.InvalidateRect();
        }
        else
        {
            // Remove the regions occupied by the control's new position
            HRGN hrgnTemp = CreateRectRgnIndirect(&rc_ctrl);
            CombineRgn(hrgnPaint, hrgnPaint, hrgnTemp, RGN_DIFF);
            DeleteObject(hrgnTemp);
            // Make the control repaint itself
            item.InvalidateRect(nullptr, true);
            item.SendMessage(WM_NCPAINT, 1, 0);
            item.Update();
        }
    }

    // Paint the newly exposed portion of the dialog box's client area
    HDC hdc = m_dlg.GetDC();
    SetDCBrushColor(hdc, GetSysColor(COLOR_3DFACE));
    FillRgn(hdc, hrgnPaint, p2p<HBRUSH>(GetStockObject(DC_BRUSH)));
    m_dlg.ReleaseDC(hdc);
    DeleteObject(hrgnPaint);
}

////////////////////////////////////////////////////////////////////////////////

void ResizeDlgLayout::ap2pt(AnchorPoint ap, int width, int height, PPOINT ppt)
{
    ppt->x = ppt->y = 0;

    switch (ap)
    {
    case AP_TOPMIDDLE:
    case AP_CENTER:
    case AP_BOTTOMMIDDLE:
        ppt->x = width / 2;
        break;

    case AP_TOPRIGHT:
    case AP_MIDDLERIGHT:
    case AP_BOTTOMRIGHT:
        ppt->x = width;
        break;

    default:
        break;
    }

    switch (ap)
    {
    case AP_MIDDLELEFT:
    case AP_CENTER:
    case AP_MIDDLERIGHT:
        ppt->y = height / 2;
        break;

    case AP_BOTTOMLEFT:
    case AP_BOTTOMMIDDLE:
    case AP_BOTTOMRIGHT:
        ppt->y = height;
        break;

    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
