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

class BaseWnd
{
public:

    HWND m_hWnd;

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE BaseWnd() : m_hWnd(nullptr)
    {
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE BaseWnd(HWND hWnd) : m_hWnd(hWnd)
    {
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE operator HWND()
    {
        return m_hWnd;
    }

    ALWAYSINLINE BaseWnd& operator=(HWND hWnd)
    {
        m_hWnd = hWnd;
        return *this;
    }

    ALWAYSINLINE bool operator==(HWND hWnd)
    {
        return m_hWnd == hWnd;
    }

    ALWAYSINLINE bool operator==(const BaseWnd& other)
    {
        return m_hWnd == other.m_hWnd;
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE bool IsWindow()
    {
        return 0 != ::IsWindow(m_hWnd);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE BaseWnd GetDlgItem(int Item)
    {
        return ::GetDlgItem(m_hWnd, Item);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE LRESULT SendMessage(UINT msg, WPARAM wp, LPARAM lp)
    {
        return ::SendMessage(m_hWnd, msg, wp, lp);
    }

    ALWAYSINLINE void PostMessage(UINT msg, WPARAM wp, LPARAM lp)
    {
        ::PostMessage(m_hWnd, msg, wp, lp);
    }

    ALWAYSINLINE LRESULT SendDlgItemMessage(int Item, UINT msg, WPARAM wp, LPARAM lp)
    {
        return ::SendDlgItemMessage(m_hWnd, Item, msg, wp, lp);
    }

    ALWAYSINLINE bool SetDlgItemText(int Item, LPCWSTR Text)
    {
        return (::SetDlgItemText(m_hWnd, Item, Text) != 0);
    }

    ALWAYSINLINE LRESULT SendNotify(NMHDR* ph)
    {
        return ::SendMessage(m_hWnd, WM_NOTIFY, ph->idFrom, p2lp(ph));
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void Destroy()
    {
        DestroyWindow(m_hWnd);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void Show()
    {
        ShowWindow(m_hWnd, SW_SHOW);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void Show(int how)
    {
        ShowWindow(m_hWnd, how);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void Hide()
    {
        ShowWindow(m_hWnd, SW_HIDE);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void Enable()
    {
        EnableWindow(m_hWnd, true);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void Enable(bool en)
    {
        EnableWindow(m_hWnd, en);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void Disable()
    {
        EnableWindow(m_hWnd, false);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void Activate()
    {
        SetActiveWindow(m_hWnd);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void SetForeground()
    {
        SetForegroundWindow(m_hWnd);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void InvalidateRect(CRect* prc = nullptr, bool Erase = false)
    {
        ::InvalidateRect(m_hWnd, prc, Erase);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void Update()
    {
        ::UpdateWindow(m_hWnd);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void Move(const CRect& r, bool Repaint = true)
    {
        ::MoveWindow(m_hWnd, r.left, r.top, r.Width(), r.Height(), Repaint);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void SetPos(
        HWND hWndInsertAfter,
        int  X,
        int  Y,
        int  cx,
        int  cy,
        UINT Flags
        )
    {
        SetWindowPos(
            m_hWnd,
            hWndInsertAfter,
            X,
            Y,
            cx,
            cy,
            Flags
            );
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void SetTopmost()
    {
        SetPos(
            HWND_TOPMOST,
            0,
            0,
            0,
            0,
            SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE
            );
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void SetNonTopmost()
    {
        SetPos(
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE
            );
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void BringToTop()
    {
        ::BringWindowToTop(m_hWnd);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void MapWindowPoint(BaseWnd* ToWnd, CPoint& pt)
    {
        ::MapWindowPoints(m_hWnd, ToWnd->m_hWnd, &pt, 1);
    }

    ALWAYSINLINE void MapWindowRect(BaseWnd* ToWnd, CRect& rc)
    {
        ::MapWindowPoints(
            m_hWnd,
            ToWnd->m_hWnd,
            p2p<POINT*>(&rc),
            2
            );
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE LPARAM LpPtToParent(LPARAM l)
    {
        CPoint pt(l);
        ::MapWindowPoints(m_hWnd, ::GetParent(m_hWnd), &pt, 1);
        return MAKELPARAM(pt.x, pt.y);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void ClientToScreen(CPoint& pt)
    {
        ::MapWindowPoints(m_hWnd, nullptr, &pt, 1);
    }

    ALWAYSINLINE void ClientToScreen(CRect& rc)
    {
        ::MapWindowPoints(m_hWnd, nullptr, p2p<POINT*>(&rc), 2);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE CRect WindowRect()
    {
        CRect rc(CRect::W(m_hWnd));
        return rc;
    }

    ALWAYSINLINE CRect ClientRect()
    {
        CRect rc(CRect::C(m_hWnd));
        return rc;
    }

    ALWAYSINLINE CRect WindowRectAsClient()
    {
        CRect rc(CRect::W(m_hWnd));
        ::MapWindowPoints(nullptr, m_hWnd, p2p<POINT*>(&rc), 2);
        return rc;
    }

    ALWAYSINLINE CRect WindowRectAsOtherClient(HWND other)
    {
        CRect rc(CRect::W(m_hWnd));
        ::MapWindowPoints(nullptr, other, p2p<POINT*>(&rc), 2);
        return rc;
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE CPoint GetCursorPos()
    {
        CPoint pt;
        ::GetCursorPos(&pt);
        ::MapWindowPoints(nullptr, m_hWnd, &pt, 1);
        return pt;
    }

    ALWAYSINLINE CPoint GetNcCursorPos()
    {
        CPoint pt;
        ::GetCursorPos(&pt);
        return (pt - WindowRect().TopLeft());
    }

    ALWAYSINLINE bool CursorOverThreadWnd()
    {
        CPoint pt;
        ::GetCursorPos(&pt);
        const DWORD tid = GetWindowThreadProcessId(WindowFromPoint(pt), nullptr);
        return (tid == GetCurrentThreadId());
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE DWORD GetDpiScale_100()
    {
        const HDC hdc = GetDC();
        const int dpi_100 = (
            GetDeviceCaps(hdc, HORZRES) * 2540 / GetDeviceCaps(hdc, HORZSIZE)
            );
        const DWORD dpi_scale_100 = (dpi_100 + 48) / 96;
        ReleaseDC(hdc);
        return dpi_scale_100;
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE DWORD GetStyle()
    {
        return GetWindowLong(m_hWnd, GWL_STYLE);
    }

    ALWAYSINLINE DWORD GetExStyle()
    {
        return GetWindowLong(m_hWnd, GWL_EXSTYLE);
    }

    ////////////////////////////////////////////////////////////////////////////

    bool ModifyStyle(
        DWORD Remove,
        DWORD Add,
        UINT Flags = 0,
        int Idx = GWL_STYLE
        )
    {
        DWORD Style = GetWindowLong(m_hWnd, Idx);
        DWORD NewStyle = (Style & ~Remove) | Add;
        if (Style == NewStyle)
        {
            return false;
        }

        SetWindowLong(m_hWnd, Idx, NewStyle);
        if (Flags != 0)
        {
            Flags |= SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER;
            SetPos(nullptr, 0, 0, 0, 0, Flags);
        }
        return true;
    }

    ALWAYSINLINE bool ModifyExStyle(DWORD Remove, DWORD Add, UINT Flags = 0)
    {
        return ModifyStyle(Remove, Add, Flags, GWL_EXSTYLE);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE HINSTANCE Hinstance()
    {
        return i2p<HINSTANCE>(GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE));
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void SetTimer(UINT_PTR Id, UINT Elapse)
    {
        ::SetTimer(m_hWnd, Id, Elapse, nullptr);
    }

    ALWAYSINLINE void KillTimer(UINT_PTR Id)
    {
        ::KillTimer(m_hWnd, Id);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void SetText(PCTSTR txt)
    {
        ::SetWindowText(m_hWnd, txt);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE Yast GetText()
    {
        return Yast(m_hWnd);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE HDC GetDC()
    {
        return ::GetDC(m_hWnd);
    }

    ALWAYSINLINE int ReleaseDC(HDC hdc)
    {
        return ::ReleaseDC(m_hWnd, hdc);
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE HFONT GetFont()
    {
        return i2p<HFONT>(SendMessage(WM_GETFONT, 0, 0));
    }

    ////////////////////////////////////////////////////////////////////////////

    ALWAYSINLINE void CheckDlgButton(int Button, bool Check)
    {
        ::CheckDlgButton(
            m_hWnd,
            Button,
            Check ? BST_CHECKED : BST_UNCHECKED
            );
    }

    ALWAYSINLINE bool IsDlgButtonChecked(int Button)
    {
        return (::IsDlgButtonChecked(m_hWnd, Button) == BST_CHECKED);
    }
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline void FillSolidRect(HDC hdc, LPCRECT pRect, COLORREF clr)
{
    const COLORREF oldclr = SetBkColor(hdc, clr);
    ExtTextOut(hdc, 0, 0, ETO_OPAQUE, pRect, nullptr, 0, nullptr);
    SetBkColor(hdc, oldclr);
}


////////////////////////////////////////////////////////////////////////////////

inline HWND CreateToolTip(HINSTANCE hInst)
{
    static const WCHAR ClassName[] = TOOLTIPS_CLASS;
    BaseWnd tt(
        CreateWindow(
            ClassName,
            nullptr,
            WS_POPUP | TTS_ALWAYSTIP,
            0,
            0,
            0,
            0,
            nullptr,
            nullptr,
            hInst,
            nullptr
            )
        );
    tt.SetTopmost();
    return tt.m_hWnd;
}

////////////////////////////////////////////////////////////////////////////////

inline bool AddToolTipText(HWND Tooltip, HWND Control, PCWSTR Text)
{
    TOOLINFO ti = { sizeof(ti) };
    ti.hwnd = Control;
    ti.uFlags = TTF_IDISHWND;
    ti.uId = p2i<UINT_PTR>(Control);
    ti.lpszText = const_cast<PWSTR>(Text);
    return (SendMessage(Tooltip, TTM_ADDTOOL, 0, p2lp(&ti)) != 0);
}

////////////////////////////////////////////////////////////////////////////////

inline bool DelToolTipText(HWND Tooltip, HWND Control)
{
    TOOLINFO ti = { sizeof(ti) };
    ti.hwnd = Control;
    ti.uId = p2i<UINT_PTR>(Control);
    return (SendMessage(Tooltip, TTM_DELTOOL, 0, p2lp(&ti)) != 0);
}

////////////////////////////////////////////////////////////////////////////////

inline void RelayToolTipMsg(HWND Tooltip, MSG *pMsg)
{
    ::SendMessage(Tooltip, TTM_RELAYEVENT, 0, p2lp(pMsg));
}

////////////////////////////////////////////////////////////////////////////////

inline bool TrayMessage(
    HWND owner,
    DWORD tray_msg,
    DWORD wm_cb_msg,
    UINT id,
    HICON h_icon = nullptr,
    LPCWSTR p_tip = nullptr
    )
{
    NOTIFYICONDATA tnd;
    tnd.cbSize = sizeof(NOTIFYICONDATA);
    tnd.hWnd = owner;
    tnd.uID = id;
    tnd.uCallbackMessage = wm_cb_msg;
    tnd.hIcon = h_icon;
    tnd.uFlags = (wm_cb_msg ? NIF_MESSAGE : 0) | (h_icon ? NIF_ICON : 0);
    if (p_tip)
    {
        tnd.uFlags |= NIF_TIP;
        sz_cpyn(tnd.szTip, p_tip, ARRAY_SIZE(tnd.szTip));
    }

    const bool res = (Shell_NotifyIcon(tray_msg, &tnd) != 0);
    if (h_icon)
    {
        DestroyIcon(h_icon);
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////