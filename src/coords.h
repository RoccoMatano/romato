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

struct CSize;
struct CPoint;
struct CRect;

////////////////////////////////////////////////////////////////////////////////

struct CSize : public SIZE
{
    CSize()
    {
        cx = 0;
        cy = 0;
    }
    CSize(int CX, int CY)
    {
        cx = CX;
        cy = CY;
    }
    CSize(SIZE Size)
    {
        *static_cast<SIZE*>(this) = Size;
    }
    CSize(POINT pt)
    {
        *p2p<POINT*>(this) = pt;
    }
    CSize(DWORD Size)
    {
        cx = static_cast<short>(LOWORD(Size));
        cy = static_cast<short>(HIWORD(Size));
    }
    bool operator==(SIZE size) const
    {
        return (cx == size.cx && cy == size.cy);
    }
    bool operator!=(SIZE size) const
    {
        return (cx != size.cx || cy != size.cy);
    }
    void operator+=(SIZE size)
    {
        cx += size.cx;
        cy += size.cy;
    }
    void operator-=(SIZE size)
    {
        cx -= size.cx;
        cy -= size.cy;
    }
    void SetSize(int CX, int CY)
    {
        cx = CX;
        cy = CY;
    }
    CSize operator+(SIZE size) const
    {
        return CSize(cx + size.cx, cy + size.cy);
    }
    CSize operator-(SIZE size) const
    {
        return CSize(cx - size.cx, cy - size.cy);
    }
    CSize operator-() const
    {
        return CSize(-cx, -cy);
    }
    ~CSize()
    {
    }

    // need other types before these can be implemented
    CPoint operator+(POINT pt) const;
    CPoint operator-(POINT pt) const;
    CRect operator+(const RECT* pRc) const;
    CRect operator-(const RECT* pRc) const;
};

////////////////////////////////////////////////////////////////////////////////

struct CPoint : public POINT
{
    CPoint()
    {
        x = 0;
        y = 0;
    }
    CPoint(int X, int Y)
    {
        x = X;
        y = Y;
    }
    CPoint(POINT pt)
    {
        *static_cast<POINT*>(this) = pt;
    }
    CPoint(SIZE Size)
    {
        *p2p<SIZE*>(this) = Size;
    }
    CPoint(LPARAM pt)
    {
        x = static_cast<short>(pt & 0xffff);
        y = static_cast<short>((pt >> 16) & 0xffff);
    }
    operator LPARAM() const
    {
        return MAKELPARAM(x, y);
    }
    void Offset(int xOffset, int yOffset)
    {
        x += xOffset;
        y += yOffset;
    }
    void Offset(POINT pt)
    {
        x += pt.x;
        y += pt.y;
    }
    void Offset(SIZE size)
    {
        x += size.cx;
        y += size.cy;
    }
    void SetPoint(int X, int Y)
    {
        x = X;
        y = Y;
    }
    bool operator==(POINT pt) const
    {
        return (x == pt.x && y == pt.y);
    }
    bool operator!=(POINT pt) const
    {
        return (x != pt.x || y != pt.y);
    }
    void operator+=(SIZE size)
    {
        x += size.cx;
        y += size.cy;
    }
    void operator-=(SIZE size)
    {
        x -= size.cx;
        y -= size.cy;
    }
    void operator+=(POINT pt)
    {
        x += pt.x;
        y += pt.y;
    }
    void operator-=(POINT pt)
    {
        x -= pt.x;
        y -= pt.y;
    }
    CPoint operator+(SIZE size) const
    {
        return CPoint(x + size.cx, y + size.cy);
    }
    CPoint operator-(SIZE size) const
    {
        return CPoint(x - size.cx, y - size.cy);
    }
    CPoint operator-() const
    {
        return CPoint(-x, -y);
    }
    CPoint operator+(POINT pt) const
    {
        return CPoint(x + pt.x, y + pt.y);
    }
    CSize operator-(POINT pt) const
    {
        return CSize(x - pt.x, y - pt.y);
    }
    ~CPoint()
    {
    }

    // need other types before these can be implemented
    CRect operator+(const RECT* pRc) const;
    CRect operator-(const RECT* pRc) const;
};

////////////////////////////////////////////////////////////////////////////////

struct CRect : public RECT
{
    CRect()
    {
        left = top = right = bottom = 0;
    }
    CRect(int l, int t, int r, int b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }
    CRect(const RECT& rc)
    {
        Copy(&rc);
    }
    CRect(LPCRECT pRc)
    {
        Copy(pRc);
    }
    CRect(POINT pt, SIZE size)
    {
        right = (left = pt.x) + size.cx;
        bottom = (top = pt.y) + size.cy;
    }
    CRect(POINT topLeft, POINT bottomRight)
    {
        left = topLeft.x;
        top = topLeft.y;
        right = bottomRight.x;
        bottom = bottomRight.y;
    }

    DECLARE_HANDLE(WINDOW_RECT);
    DECLARE_HANDLE(CLIENT_RECT);
    DECLARE_HANDLE(NOINIT_RECT);
    static WINDOW_RECT W(HWND hWnd)
    {
        return p2p<WINDOW_RECT>(hWnd);
    }
    static CLIENT_RECT C(HWND hWnd)
    {
        return p2p<CLIENT_RECT>(hWnd);
    }
    static NOINIT_RECT _NoInit()
    {
        return typed_null<NOINIT_RECT>();
    }

    CRect(WINDOW_RECT hWnd)
    {
        ::GetWindowRect(p2p<HWND>(hWnd), this);
    }
    CRect(CLIENT_RECT hWnd)
    {
        ::GetClientRect(p2p<HWND>(hWnd), this);
    }
    CRect(NOINIT_RECT)
    {
    }
    CRect(WINDOWPOS* pWp)
    {
        left = pWp->x;
        top = pWp->y;
        right = pWp->x + pWp->cx;
        bottom = pWp->y + pWp->cy;
    }
    static CRect& FromLPARAM(LPARAM l)
    {
        return *(i2p<CRect*>(l));
    }
    int Width() const
    {
        return right - left;
    }
    int Height() const
    {
        return bottom - top;
    }
    CSize Size() const
    {
        return CSize(right - left, bottom - top);
    }
    CPoint& TopLeft()
    {
        return *(p2p<CPoint*>(this));
    }
    CPoint& BottomRight()
    {
        return *(p2p<CPoint*>(this) + 1);
    }
    const CPoint& TopLeft() const
    {
        return *(p2p<const CPoint*>(this));
    }
    const CPoint& BottomRight() const
    {
        return *(p2p<const CPoint*>(this) + 1);
    }
    CPoint CenterPoint() const
    {
        return CPoint((left + right) / 2, (top + bottom) / 2);
    }
    void SwapLeftRight()
    {
        LONG temp = left;
        left = right;
        right = temp;
    }
    operator LPRECT()
    {
        return this;
    }
    operator LPCRECT() const
    {
        return this;
    }

    bool IsEmpty() const
    {
        return (left >= right) || (top >= bottom);
    }
    bool IsNull() const
    {
        return (left == 0 && right == 0 && top == 0 && bottom == 0);
    }
    bool PtInRect(POINT pt) const
    {
        return (
            (pt.x >= left && pt.x < right) &&
            ((pt.y >= top && pt.y < bottom))
            );
    }
    void FromWindow(HWND hWnd)
    {
        ::GetWindowRect(hWnd, this);
    }
    void FromClient(HWND hWnd)
    {
        ::GetClientRect(hWnd, this);
    }
    void SetRect(int l, int t, int r, int b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }
    void SetRect(POINT topLeft, POINT bottomRight)
    {
        left = topLeft.x;
        top = topLeft.y;
        right = bottomRight.x;
        bottom = bottomRight.y;
    }
    void SetEmpty()
    {
        left = right = top = bottom = 0;
    }
    void Copy(LPCRECT pRc)
    {
        left = pRc->left;
        top = pRc->top;
        right = pRc->right;
        bottom = pRc->bottom;
    }
    bool Equal(LPCRECT pRc) const
    {
        return (
            left == pRc->left &&
            top == pRc->top &&
            right == pRc->right &&
            bottom == pRc->bottom
            );
    }
    void Inflate(int i)
    {
        left -= i;
        right += i;
        top -= i;
        bottom += i;
    }
    void Inflate(int x, int y)
    {
        left -= x;
        right += x;
        top -= y;
        bottom += y;
    }
    void Inflate(SIZE size)
    {
        Inflate(size.cx, size.cy);
    }
    void Inflate(LPCRECT pRc)
    {
        left -= pRc->left;
        top -= pRc->top;
        right += pRc->right;
        bottom += pRc->bottom;
    }
    void Inflate(int l, int t, int r, int b)
    {
        left -= l;
        top -= t;
        right += r;
        bottom += b;
    }
    void Deflate(int i)
    {
        Inflate(-i);
    }
    void Deflate(int x, int y)
    {
        Inflate(-x, -y);
    }
    void Deflate(SIZE size)
    {
        Inflate(-size.cx, -size.cy);
    }
    void Deflate(LPCRECT pRc)
    {
        left += pRc->left;
        top += pRc->top;
        right -= pRc->right;
        bottom -= pRc->bottom;
    }
    void Deflate(int l, int t, int r, int b)
    {
        left += l;
        top += t;
        right -= r;
        bottom -= b;
    }
    void Offset(int x, int y)
    {
        left += x;
        right += x;
        top += y;
        bottom += y;
    }
    void Offset(SIZE size)
    {
        Offset(size.cx, size.cy);
    }
    void Offset(POINT pt)
    {
        Offset(pt.x, pt.y);
    }
    void Normalize()
    {
        if (left > right)
        {
            int tmp = left;
            left = right;
            right = tmp;
        }
        if (top > bottom)
        {
            int tmp = top;
            top = bottom;
            bottom = tmp;
        }
    }
    void MoveToY(int y)
    {
        bottom = Height() + y;
        top = y;
    }
    void MoveToX(int x)
    {
        right = Width() + x;
        left = x;
    }
    void MoveToXY(int x, int y)
    {
        MoveToX(x);
        MoveToY(y);
    }
    void MoveToXY(POINT pt)
    {
        MoveToX(pt.x);
        MoveToY(pt.y);
    }
    void Intersection(LPCRECT a, LPCRECT b)
    {
        left = a->left > b->left ? a->left : b->left;
        top = a->top > b->top ? a->top : b->top;
        right = a->right < b->right ? a->right : b->right;
        bottom = a->bottom < b->bottom ? a->bottom : b->bottom;
    }
    bool IntersectRect(LPCRECT a, LPCRECT b)
    {
        Intersection(a, b);
        return !IsEmpty();
    }
    void Union(LPCRECT a, LPCRECT b)
    {
        left = a->left < b->left ? a->left : b->left;
        top = a->top < b->top ? a->top : b->top;
        right = a->right > b->right ? a->right : b->right;
        bottom = a->bottom > b->bottom ? a->bottom : b->bottom;
    }
    bool UnionRect(LPCRECT a, LPCRECT b)
    {
        Union(a, b);
        return !IsEmpty();
    }
    void Subtraction(LPCRECT a, LPCRECT b)
    {
        Copy(a);
        CRect irc(_NoInit());
        if (irc.IntersectRect(a, b))
        {
            LONG cnt = 0;
            LONG ileft = irc.left;
            if (ileft <= left)
            {
                cnt += 1;
            }
            LONG itop = irc.top;
            if (itop <= top)
            {
                cnt += 1;
            }
            LONG iright = irc.right;
            if (iright >= right)
            {
                cnt += 1;
            }
            LONG ibottom = irc.bottom;
            if (ibottom >= bottom)
            {
                cnt += 1;
            }

            if (cnt == 4)
            {
                left = top = right = bottom = 0;
            }
            else if (cnt != 3)
            {
                return;
            }
            else if (ileft > left)
            {
                right = ileft;
            }
            else if (iright < right)
            {
                left = iright;
            }
            else if (itop > top)
            {
                bottom = itop;
            }
            else if (ibottom < bottom)
            {
                top = ibottom;
            }
        }
    }
    bool SubtractRect(LPCRECT a, LPCRECT b)
    {
        Subtraction(a, b);
        return !IsEmpty();
    }

    void operator=(const RECT& rc)
    {
        Copy(&rc);
    }
    bool operator==(const RECT& rc) const
    {
        return Equal(&rc);
    }
    bool operator!=(const RECT& rc) const
    {
        return !Equal(&rc);
    }
    void operator+=(POINT pt)
    {
        Offset(pt.x, pt.y);
    }
    void operator+=(SIZE size)
    {
        Offset(size.cx, size.cy);
    }
    void operator+=(LPCRECT pRc)
    {
        Inflate(pRc);
    }
    void operator-=(POINT pt)
    {
        Offset(-pt.x, -pt.y);
    }
    void operator-=(SIZE size)
    {
        Offset(-size.cx, -size.cy);
    }
    void operator-=(LPCRECT pRc)
    {
        Deflate(pRc);
    }
    void operator&=(const RECT& rc)
    {
        IntersectRect(this, &rc);
    }
    void operator|=(const RECT& rc)
    {
        UnionRect(this, &rc);
    }
    CRect operator+(POINT pt) const
    {
        CRect rc(*this);
        rc.Offset(pt.x, pt.y);
        return rc;
    }
    CRect operator-(POINT pt) const
    {
        CRect rc(*this);
        rc.Offset(-pt.x, -pt.y);
        return rc;
    }
    CRect operator+(LPCRECT pRc) const
    {
        CRect rc(this);
        rc.Inflate(pRc);
        return rc;
    }
    CRect operator+(SIZE size) const
    {
        CRect rc(*this);
        rc.Offset(size.cx, size.cy);
        return rc;
    }
    CRect operator-(SIZE size) const
    {
        CRect rc(*this);
        rc.Offset(-size.cx, -size.cy);
        return rc;
    }
    CRect operator-(LPCRECT pRc) const
    {
        CRect rc(this);
        rc.Deflate(pRc);
        return rc;
    }
    CRect operator&(const RECT& rect2) const
    {
        CRect rc(_NoInit());
        rc.IntersectRect(this, &rect2);
        return rc;
    }
    CRect operator|(const RECT& rect2) const
    {
        CRect rc(_NoInit());
        rc.UnionRect(this, &rect2);
        return rc;
    }
    ~CRect()
    {
    }
};

////////////////////////////////////////////////////////////////////////////////

inline CPoint CSize::operator+(POINT pt) const
{
    return CPoint(cx + pt.x, cy + pt.y);
}

inline CPoint CSize::operator-(POINT pt) const
{
    return CPoint(cx - pt.x, cy - pt.y);
}

inline CRect CSize::operator+(const RECT* pRc) const
{
    return CRect(pRc) + *this;
}

inline CRect CSize::operator-(const RECT* pRc) const
{
    return CRect(pRc) - *this;
}

////////////////////////////////////////////////////////////////////////////////

inline CRect CPoint::operator+(const RECT* pRc) const
{
    return CRect(pRc) + *this;
}
inline CRect CPoint::operator-(const RECT* pRc) const
{
    return CRect(pRc) - *this;
}

////////////////////////////////////////////////////////////////////////////////
