/* -*-plt-c++-*- */
#ifndef PLT_SORT_INCLUDED
#define PLT_SORT_INCLUDED
/*
 * Copyright (c) 1996, 1997, 1998, 1999
 * Lehrstuhl fuer Prozessleittechnik, RWTH Aachen
 * D-52064 Aachen, Germany.
 * All rights reserved.
 *
 * This file is part of the ACPLT/KS Package which is licensed as open
 * source under the Artistic License; you can use, redistribute and/or
 * modify it under the terms of that license.
 *
 * You should have received a copy of the Artistic License along with
 * this Package; see the file ARTISTIC-LICENSE. If not, write to the
 * Copyright Holder.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
 * OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/* Author: Markus Juergens <markusj@plt.rwth-aachen.de> */

//////////////////////////////////////////////////////////////////////

#include "plt/array.h"

//////////////////////////////////////////////////////////////////////
// sort array toSort,
// class T must support operator <
//
template <class T>
class PltSort
{
public:
    static void qsort(PltArray<T> &toSort);
protected:
    static void _swap(T&, T&);
    static void _sort2(T&, T&);
    static void _sort3(T&, T&, T&);
    static void _rek_qsort(PltArray<T> &, size_t, size_t);
};

//////////////////////////////////////////////////////////////////////
// INLINE IMPLEMENTATION
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// swap two elements
//
template <class T> 
inline void
PltSort<T>::_swap(T &a, T &b)
{
    T temp(a);
    a = b;
    b = temp;
}

//////////////////////////////////////////////////////////////////////

#if PLT_SEE_ALL_TEMPLATES
#include "plt/sort_impl.h"
#endif

//////////////////////////////////////////////////////////////////////

#endif // PLT_SORT_INCLUDED









