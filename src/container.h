////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the romato library.
//
// Copyright 2013-2022 Rocco Matano
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

///////////////////////////////////////////////////////////////////////////////

// a custom allocator

template <class T> struct CustAll // because it smells like Kuhstall!
{
    typedef T value_type;

    CustAll()
    {
    }

    template<class U> CustAll(const CustAll<U>&)
    {
    }

    template<class U> bool operator==(const CustAll<U>&) const
    {
        return true;
    }

    template<class U> bool operator!=(const CustAll<U>&) const
    {
        return false;
    }

    T* allocate(const size_t n) const
    {
        return static_cast<T*>(malloc(n * sizeof(T)));
    }

    void deallocate(T* const p, size_t) const
    {
        free(p);
    }
};

///////////////////////////////////////////////////////////////////////////////

#include <vector>
template<class T> using cvector = std::vector<T, CustAll<T>>;

#include <list>
template<class T> using clist = std::list<T, CustAll<T>>;

#include <deque>
template<class T> using cdeque = std::deque<T, CustAll<T>>;

#include <set>
template<class T> using cset = std::set<T, std::less<T>, CustAll<T>>;

#include <map>
template<class Key, class Value> using cmap = std::map<
    Key,
    Value,
    std::less<Key>,
    CustAll<std::pair<const Key, Value>>
    >;

#include <unordered_map>
template<class Key, class Value> using cumap = std::unordered_map<
    Key,
    Value,
    std::hash<Key>,
    std::equal_to<Key>,
    CustAll<std::pair<const Key, Value>>
    >;

////////////////////////////////////////////////////////////////////////////////

// iterator for anything that has an array memory layout (i.e. address of
// element N+1 is equal to (address of element N) + sizeof(element))

template <class T> class array_const_iterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type      = const T;
    using difference_type = ptrdiff_t;
    using pointer         = const T*;
    using reference       = const value_type&;

    array_const_iterator() : m_ptr()
    {
    }

    array_const_iterator(pointer ptr) : m_ptr(ptr)
    {
    }

    reference operator*() const
    {
        return *m_ptr;
    }

    pointer operator->() const
    {
        return m_ptr;
    }

    array_const_iterator& operator++()
    {
        ++m_ptr;
        return *this;
    }

    array_const_iterator operator++(int)
    {
        array_const_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    array_const_iterator& operator--()
    {
        --m_ptr;
        return *this;
    }

    array_const_iterator operator--(int)
    {
        array_const_iterator tmp = *this;
        --*this;
        return tmp;
    }

    array_const_iterator& operator+=(const difference_type offs)
    {
        m_ptr += offs;
        return *this;
    }

    array_const_iterator operator+(const difference_type offs) const
    {
        array_const_iterator tmp = *this;
        return tmp += offs;
    }

    array_const_iterator& operator-=(const difference_type offs)
    {
        return *this += -offs;
    }

    array_const_iterator operator-(const difference_type offs) const
    {
        array_const_iterator tmp = *this;
        return tmp -= offs;
    }

    difference_type operator-(const array_const_iterator& rhs) const
    {
        return m_ptr - rhs.m_ptr;
    }

    reference operator[](const difference_type offs) const
    {
        return *(*this + offs);
    }

    bool operator==(const array_const_iterator& rhs) const
    {
        return m_ptr == rhs.m_ptr;
    }

    bool operator!=(const array_const_iterator& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator<(const array_const_iterator& rhs) const
    {
        return m_ptr < rhs.m_ptr;
    }

    bool operator>(const array_const_iterator& rhs) const
    {
        return rhs < *this;
    }

    bool operator<=(const array_const_iterator& rhs) const
    {
        return !(rhs < *this);
    }

    bool operator>=(const array_const_iterator& rhs) const
    {
        return !(*this < rhs);
    }

    pointer m_ptr;
};

template <class T> inline array_const_iterator<T>
operator+(ptrdiff_t offs, array_const_iterator<T> next)
{
    return next += offs;
}

////////////////////////////////////////////////////////////////////////////////

template <class T> class array_iterator : public array_const_iterator<T>
{
public:
    using base_class        = array_const_iterator<T>;
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = T;
    using difference_type   = base_class::difference_type;
    using pointer           = T*;
    using reference         = value_type&;

    array_iterator()
    {
    }

    array_iterator(pointer ptr) : base_class(ptr)
    {
    }

    reference operator*() const
    {
        return const_cast<reference>(base_class::operator*());
    }

    pointer operator->() const
    {
        return const_cast<pointer>(base_class::operator->());
    }

    array_iterator& operator++()
    {
        ++*(static_cast<base_class*>(this));
        return *this;
    }

    array_iterator operator++(int)
    {
        array_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    array_iterator& operator--()
    {
        --*(static_cast<base_class*>(this));
        return *this;
    }

    array_iterator operator--(int)
    {
        array_iterator tmp = *this;
        --*this;
        return tmp;
    }

    array_iterator& operator+=(const difference_type offs)
    {
        *(static_cast<base_class*>(this)) += offs;
        return *this;
    }

    array_iterator operator+(const difference_type offs) const
    {
        array_iterator tmp = *this;
        return tmp += offs;
    }

    array_iterator& operator-=(const difference_type offs)
    {
        return *this += -offs;
    }

    array_iterator operator-(const difference_type offs) const
    {
        array_iterator tmp = *this;
        return tmp -= offs;
    }

    difference_type operator-(const base_class& rhs) const
    {
        return *(static_cast<const base_class*>(this)) - rhs;
    }

    reference operator[](const difference_type offs) const
    {
        return *(*this + offs);
    }
};

template <class T> inline array_iterator<T>
operator+(ptrdiff_t offs, array_iterator<T> next)
{
    return next += offs;
}

////////////////////////////////////////////////////////////////////////////////
