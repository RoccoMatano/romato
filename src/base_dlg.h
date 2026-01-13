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

#define MSDN_FIRST               (0U-4000U)       // ModelesS Dialog
#define MSDN_LAST                (0U-4050U)

#define MSDN_ACTIVATE (MSDN_FIRST + 1)
struct NM_MSD_ACTIVATE
{
    NMHDR   hdr;
    BOOL    is_active;
};

#define MSDN_DESTROY (MSDN_FIRST + 2)
#define NM_MSD_DESTROY NMHDR


// Since this class is only meant for inheritance and not for instantiation,
// save same bytes by omitting the vtable

class NO_VTABLE BaseDlg : public BaseWnd
{
protected:

    BaseWnd* m_pParent;

    ////////////////////////////////////////////////////////////////////////////
    //
    // Even if this code was only meant for modeless dialogs:
    // MUST NOT call CreateModeless() inside the constructor of this base class!
    // That would create the call chain
    // ctor -> CreateModeless -> CreateDialogParam -> DlgProc -> OnInitDialog
    //
    // Since the object is still a BaseDlg and OnInitDialog is declared virtual,
    // the empty base class implementation of OnInitDialog would be called -
    // which would be bad enough in itself.
    // But even that would only work, if there was a valid vftable ptr. Since
    // this class is declared as having no vtable, that vftable ptr is nullptr
    // and DlgProc will crash.
    //
    // Fortunately it is no problem to call CreateModeless in the constructor of
    // a derived class.
    //
    ////////////////////////////////////////////////////////////////////////////

    BaseDlg(BaseWnd* Parent) :
        BaseWnd(),
        m_pParent(Parent)
    {
    }

    ////////////////////////////////////////////////////////////////////////////

    // modeless

    void CreateModeless(PCWSTR TmplName, HINSTANCE hinst = GetCallerHinstance())
    {
        if (
            nullptr == CreateDialogParam(
                hinst,
                TmplName,
                m_pParent ? m_pParent->m_hWnd : nullptr,
                DlgProc,
                p2lp(this)
                )
            )
        {
            RaiseException(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    void CreateModeless(UINT TmplId, HINSTANCE hinst = GetCallerHinstance())
    {
        CreateModeless(MAKEINTRESOURCE(TmplId), hinst);
    }

    // modal

    INT_PTR DoModal(PCWSTR TmplName, HINSTANCE hinst = GetCallerHinstance())
    {
        return DialogBoxParam(
            hinst,
            TmplName,
            m_pParent ? m_pParent->m_hWnd : nullptr,
            DlgProc,
            p2lp(this)
            );
    }

    INT_PTR DoModal(UINT TmplId, HINSTANCE hinst = GetCallerHinstance())
    {
        return DoModal(MAKEINTRESOURCE(TmplId), hinst);
    }

    ////////////////////////////////////////////////////////////////////////////

    static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

    virtual INT_PTR OnMessage(UINT msg, WPARAM wp, LPARAM lp)
    {
        UNREFERENCED_PARAMETER(msg);
        UNREFERENCED_PARAMETER(wp);
        UNREFERENCED_PARAMETER(lp);
        return false;
    }

    virtual bool OnInitDialog()
    {
        return true;
    }

    virtual bool OnCommand(UINT CmdId, UINT Notification, HWND Ctrl)
    {
        UNREFERENCED_PARAMETER(CmdId);
        UNREFERENCED_PARAMETER(Notification);
        UNREFERENCED_PARAMETER(Ctrl);
        return false;
    }

    virtual bool OnNotify(UINT CtrlId, NMHDR *pHdr)
    {
        if (m_pParent && pHdr->code == MSDN_ACTIVATE)
        {
            m_pParent->SendNotify(pHdr);
            return true;
        }
        UNREFERENCED_PARAMETER(CtrlId);
        return false;
    }

    virtual ~BaseDlg()
    {
        // In DlgProc we set m_hWnd to nullptr when WM_NCDESTROY is processed.
        // So either WM_NCDESTROY was processed (i.e. this window already has
        // been destroyed and the following DestroyWindow will operate on a
        // window handle of nullptr having no effect) or this window still needs
        // to be destroyed, before we can allow the memory for this object to
        // vanish.
        Destroy();
    }

    BaseWnd GetItem(int Id)
    {
        return GetDlgItem(Id);
    }

    Yast GetText(int id)
    {
        return Yast(GetItem(id));
    }

    void GetText(int id, Yast& dst)
    {
        dst = GetText(id);
    }

    bool IsButtonChecked(int id)
    {
        return IsDlgButtonChecked(id);
    }

    void CheckButton(int id, bool check)
    {
        CheckDlgButton(id, check);
    }

    void CheckRadioButton(int first, int last, int check)
    {
        ::CheckRadioButton(m_hWnd, first, last, check);
    }

    void SendDestroyRequest()
    {
        if (m_pParent)
        {
            NM_MSD_DESTROY md = {m_hWnd, 0, MSDN_DESTROY};
            m_pParent->SendNotify(&md);
        }
        else
        {
            Destroy();
        }
    }

    void SetMsgResult(LRESULT result)
    {
        SetWindowLongPtr(m_hWnd, DWLP_MSGRESULT, result);
    }

    // This class represents objects (via m_hWnd) that can not be duplicated
    // by simply copying memory. Therefore we have to inhibit copying objects.
    BaseDlg(const BaseDlg& src) = delete;
    BaseDlg& operator=(BaseDlg& src) = delete;

};
