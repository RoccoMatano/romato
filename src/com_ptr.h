////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the romato library.
//
// Copyright 2013-2023 Rocco Matano
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

#include "unknwn.h"

template <class T>
class _InhibitAddRefRelease : public T
{
private:
    STDMETHOD_(ULONG, AddRef)() = 0;
    STDMETHOD_(ULONG, Release)() = 0;
};

template <class T>
class ComPtr
{
protected:
    T* ptr;

    void swap(ComPtr& other)
    {
        T* tmp = other.ptr;
        other.ptr = ptr;
        ptr = tmp;
    }

public:
    ComPtr() : ptr(nullptr)
    {
    }

    ComPtr(T* pt) : ptr(pt)
    {
        if (ptr != nullptr)
        {
            ptr->AddRef();
        }
    }

    ComPtr(const ComPtr<T>& rt) : ptr(rt.ptr)
    {
        if (ptr != nullptr)
        {
            ptr->AddRef();
        }
    }

    ComPtr(ComPtr<T>&& rrt)
    {
        rrt.swap(*this);
    }

    ~ComPtr()
    {
        Release();
    }

    T* operator=(T* pt)
    {
        if (ptr != pt)
        {
            ComPtr(pt).swap(*this);
        }
        return *this;
    }

    T* operator=(const ComPtr<T>& rt)
    {
        if (ptr != rt.ptr)
        {
            ComPtr(rt).swap(*this);
        }
        return *this;
    }

    T* operator=(ComPtr<T>&& rrt)
    {
        if (ptr != rrt.ptr)
        {
            rrt.swap(*this);
        }
        return *this;
    }

    operator T*() const
    {
        return ptr;
    }

    T& operator*() const
    {
        return *ptr;
    }

    T** operator&()
    {
        Release();
        return &ptr;
    }

    _InhibitAddRefRelease<T>* operator->() const
    {
        return reinterpret_cast<_InhibitAddRefRelease<T>*>(ptr);
    }

    bool operator!() const
    {
        return (ptr == nullptr);
    }

    bool operator<(T* pt) const
    {
        return ptr < pt;
    }

    bool operator!=(T* pt) const
    {
        return !operator==(pt);
    }

    bool operator==(T* pt) const
    {
        return ptr == pt;
    }

    void Release()
    {
        if (ptr)
        {
            // avoid reentrancy issues
            T* tmp = ptr;
            ptr = nullptr;
            tmp->Release();
        }
    }

    template <class O>
    HRESULT QueryInterface(O** pp) const
    {
        return ptr->QueryInterface(__uuidof(O), reinterpret_cast<void**>(pp));
    }
};
