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

#include "base_dlg.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class NO_VTABLE FontSizeDlg : public BaseDlg
{
protected:

    // This class does not use the dialog templates from the resources directly.
    // Instead it changes the font size in the template before actually creating
    // the dialog. The parameter 'FontSize' controls how the font size is
    // manipulated:
    //
    // FontSize > 0: FontSize is used directly as the new size.
    // FontSize == 0: The font size of the dialog template is left unchanged.
    // FontSize < 0: The original font size will be scaled by abs(FontSize),
    //      where abs(FontSize) is interpreted as a percentage.
    //      new_size = (original_size * abs(FontSize) + 50) / 100
    //

    FontSizeDlg(BaseWnd* Parent) : BaseDlg(Parent)
    {
    }

    ////////////////////////////////////////////////////////////////////////////
    // modeless

    void CreateSizedModeless(PCWSTR TmplName, UINT FontSize, HINSTANCE hinst);
    INT_PTR DoSizedModal(PCWSTR TmplName, UINT FontSize, HINSTANCE hinst);

    void CreateModeless(
        PCWSTR TmplName,
        int FontSize = 0,
        HINSTANCE hinst = GetCallerHinstance()
        )
    {
        CreateSizedModeless(TmplName, FontSize, hinst);
    }

    void CreateModeless(
        UINT TmplId,
        UINT FontSize = 0,
        HINSTANCE hinst = GetCallerHinstance()
        )
    {
        CreateSizedModeless(MAKEINTRESOURCE(TmplId), FontSize, hinst);
    }

    ////////////////////////////////////////////////////////////////////////////
    // modal

    INT_PTR DoModal(
        PCWSTR TmplName,
        UINT FontSize = 0,
        HINSTANCE hinst = GetCallerHinstance()
        )
    {
        return DoSizedModal(TmplName, FontSize, hinst);
    }

    INT_PTR DoModal(
        UINT TmplId,
        UINT FontSize = 0,
        HINSTANCE hinst = GetCallerHinstance()
        )
    {
        return DoSizedModal(MAKEINTRESOURCE(TmplId), FontSize, hinst);
    }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class NO_VTABLE DpiScaledDlg : public FontSizeDlg
{
protected:

    DpiScaledDlg(BaseWnd* Parent) : FontSizeDlg(Parent)
    {
    }

    ////////////////////////////////////////////////////////////////////////////
    // Actual font size is calculated to deliver a similar height as on a
    // device with 96 DPI.

    int CalcFontSize()
    {
        BaseWnd dpi_wnd(m_pParent ? m_pParent->m_hWnd : nullptr);
        return -static_cast<int>(dpi_wnd.GetDpiScale_100());
    }


    ////////////////////////////////////////////////////////////////////////////
    // modeless

    void CreateModeless(PCWSTR TmplName, HINSTANCE hinst = GetCallerHinstance())
    {
        FontSizeDlg::CreateModeless(TmplName, CalcFontSize(), hinst);
    }

    void CreateModeless(UINT TmplId, HINSTANCE hinst = GetCallerHinstance())
    {
        FontSizeDlg::CreateModeless(TmplId, CalcFontSize(), hinst);
    }

    ////////////////////////////////////////////////////////////////////////////
    // modal

    INT_PTR DoModal(PCWSTR TmplName, HINSTANCE hinst = GetCallerHinstance())
    {
        return FontSizeDlg::DoModal(TmplName, CalcFontSize(), hinst);
    }

    INT_PTR DoModal(UINT TmplId, HINSTANCE hinst = GetCallerHinstance())
    {
        return FontSizeDlg::DoModal(TmplId, CalcFontSize(), hinst);
    }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
