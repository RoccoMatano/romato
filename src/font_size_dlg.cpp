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

#include "romato.h"
#include "font_size_dlg.h"

////////////////////////////////////////////////////////////////////////////////

struct DLGTEMPLATEEX
{
    WORD dlgVer;
    WORD signature;
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    WORD cDlgItems;
    WORD x;
    WORD y;
    WORD cx;
    WORD cy;

    // Everything else in this structure is variable length,
    // and therefore must be determined dynamically

    // sz_Or_Ord menu;          // name or ordinal of a menu resource
    // sz_Or_Ord windowClass;   // name or ordinal of a window class
    // WCHAR title[titleLen];   // title string of the dialog box
    // WORD pointsize;          // only if DS_SETFONT is set
    // WORD weight;             // only if DS_SETFONT is set
    // WORD bItalic;            // only if DS_SETFONT is set
    // WCHAR font[fontLen];     // typeface name, if DS_SETFONT is set
};

////////////////////////////////////////////////////////////////////////////////
//
// LoadDlgFontSize loads a dialog template from resources and then changes
// the font size that was stored in the template. It can do so in two
// different ways:
//
// a) The new font size can be supplied directly (e.g size = 10).
//
// b) A scaling factor can be supplied that is applied to the size stored in
//    the dialog template in the resources (e.g. scale = 120%).
//
// The parameter FontSize is used to supply the required value *and* to choose
// the method:
//
// FontSize > 0: FontSize is used directly as the new size.
// FontSize == 0: The font size of the dialog template is left unchanged.
// FontSize < 0: The original font size will be scaled by abs(FontSize),
//      where abs(FontSize) is interpreted as a percentage.
//      new_size = (original_size * abs(FontSize) + 50) / 100
//
// Since resources are mapped into read-only memory, they cannot be changed
// directly. Instead a copy has to be created. LoadDlgFontSize returns
// such a changed copy, that has been allocated with 'malloc' and it is
// the duty of the caller to release it by calling 'free'.
//
////////////////////////////////////////////////////////////////////////////////

static DLGTEMPLATE* LoadDlgFontSize(
    int FontSize,
    PCWSTR TmplName,
    HINSTANCE hinst
    )
{
    HRSRC hres = FindResource(hinst, TmplName, RT_DIALOG);
    DWORD len  = SizeofResource(hinst, hres);
    void *data = static_cast<void*>(LoadResource(hinst, hres));
    if (data == nullptr)
    {
        RaiseException(HRESULT_FROM_WIN32(GetLastError()));
    }
    DLGTEMPLATE *pDlg = static_cast<DLGTEMPLATE*>(malloc(len));
    memcpy(pDlg, data, len);

    if (FontSize == 0)
    {
        return pDlg;
    }

    DWORD style;
    PWORD pWord;
    DLGTEMPLATEEX *pExDlg = p2p<DLGTEMPLATEEX*>(pDlg);
    if (pExDlg->signature != 0xFFFF)
    {
        // A regular DLGTEMPLATE
        style = pDlg->style;
        pWord = p2p<PWORD>(pDlg + 1);
    }
    else
    {
        style = pExDlg->style;
        pWord = &(pExDlg->cy) + 1;
    }
    if (!(style & DS_SETFONT))
    {
        // dialog template does not have a font size
        return pDlg;
    }

    // skip past menu
    if (*pWord != 0x00FF)
    {
        while (*pWord) pWord++;
        pWord++;
    }
    else
    {
        pWord += 2;
    }

    // skip past class
    if (*pWord != 0x00FF)
    {
        while (*pWord) pWord++;
        pWord++;
    }
    else
    {
        pWord += 2;
    }

    // skip past title
    while (*pWord) pWord++;
    pWord++;

    // now pWord points to font size
    if (FontSize > 0)
    {
        *pWord = static_cast<WORD>(FontSize);
    }
    else
    {
        *pWord = static_cast<WORD>((((*pWord) * -FontSize) + 50) / 100);
    }
    return pDlg;
}

////////////////////////////////////////////////////////////////////////////////

void FontSizeDlg::CreateSizedModeless(
    PCWSTR TmplName,
    UINT FontSize,
    HINSTANCE hinst
    )
{
    DLGTEMPLATE *pTmpl = LoadDlgFontSize(FontSize, TmplName, hinst);
    const HWND hwnd = CreateDialogIndirectParam(
        hinst,
        pTmpl,
        m_pParent ? m_pParent->m_hWnd : nullptr,
        DlgProc,
        p2lp(this)
        );
    free(pTmpl);
    if (hwnd == nullptr)
    {
        RaiseException(HRESULT_FROM_WIN32(GetLastError()));
    }
}

////////////////////////////////////////////////////////////////////////////////

INT_PTR FontSizeDlg::DoSizedModal(
    PCWSTR TmplName,
    UINT FontSize,
    HINSTANCE hinst
    )
{
    DLGTEMPLATE *pTmpl = LoadDlgFontSize(FontSize, TmplName, hinst);
    INT_PTR res = DialogBoxIndirectParam(
        hinst,
        pTmpl,
        m_pParent ? m_pParent->m_hWnd : nullptr,
        DlgProc,
        p2lp(this)
        );
    free(pTmpl);
    return res;
}

////////////////////////////////////////////////////////////////////////////////
