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

#include "base_wnd.h"

enum AnchorPoint
{
    AP_TOPLEFT,
    AP_TOPMIDDLE,
    AP_TOPRIGHT,
    AP_MIDDLERIGHT,
    AP_BOTTOMRIGHT,
    AP_BOTTOMMIDDLE,
    AP_BOTTOMLEFT,
    AP_MIDDLELEFT,
    AP_CENTER
};


class ResizeDlgLayout {
public:

    // Set dialog and mininum size. If minimum width or height are not supplied,
    // the original size is taken as minimum size.
    void Initialize(BaseWnd dlg, int min_width = 0, int min_height = 0);

    // For each and every control in your derived dialog you should define
    // how it should react on a change of the dialog size. You do so by calling
    // 'AnchorControl(s)'. The parameter 'upper_left' states to which part of
    // the dialog the controls upper left corner will stay in a constant
    // relative posistion. 'lower_right' does the same for the lower right
    // corner (of course).
    //
    // Not calling 'AnchorControl' for a control will have the same result as
    // calling  AnchorControl(AP_TOPLEFT, AP_TOPLEFT, ...).
    //
    // If you have a long list of controls, you might find it more convenient
    // to use AnchorControls(). This function is provided with an array of
    // CtrlAnchor objects.
    //
    // Both flavours of AnchorControl will only work, if they are called during
    // WM_INITDIALOG.
    //
    // When repositioning a control, complete repainting of the control can be
    // forced by setting 'redraw' to true, when setting it anchor points.
    // This is not necessary for group boxes as these always need to be
    // repainted and this class already takes care of that.

    bool AnchorControl(
        AnchorPoint upper_left,
        AnchorPoint lower_right,
        int id,
        bool redraw = false
        );

    struct CtrlAnchor
    {
        int id;
        AnchorPoint upper_left;
        AnchorPoint lower_right;
        bool redraw;
    };
    bool AnchorControls(const CtrlAnchor *anchors, int num_anchors);

    // dlg has to call this in its WM_SIZE handler
    void OnSize(int cx, int cy);

    // dlg should call this in its WM_GETMINMAXINFO handler
    void OnMinMaxInfo(PMINMAXINFO pMinMax)
    {
        pMinMax->ptMinTrackSize = m_min_parent_dims;
    }

private:
    struct CtrlInfo
    {
        POINT       ul_delta;
        POINT       lr_delta;
        AnchorPoint upper_left;
        AnchorPoint lower_right;
        int         id;
        bool        redraw;
    };

    static void ap2pt(AnchorPoint ap, int width, int height, PPOINT ppt);

    clist<CtrlInfo> m_ctrl_data;
    BaseWnd         m_dlg;
    CPoint          m_min_parent_dims;
};
