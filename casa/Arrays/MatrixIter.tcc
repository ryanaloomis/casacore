//# MatrixIter.cc: Iterate a matrix cursor through another array
//# Copyright (C) 1993,1994,1995
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA

#ifndef CASA_MATRIXITER_2_TCC
#define CASA_MATRIXITER_2_TCC

#include "MatrixIter.h"

namespace casacore { //# NAMESPACE CASACORE - BEGIN

template<typename T, typename Alloc>
MatrixIterator<T, Alloc>::MatrixIterator(Array<T, Alloc> &a)
: ArrayIterator<T, Alloc>(a, 2)
{
    // We need to ensure that ap points at a Matrix
    this->ap_p.reset( new Matrix<T, Alloc>(*this->ap_p) ); // reference
}

template<typename T, typename Alloc>
MatrixIterator<T, Alloc>::MatrixIterator(Array<T, Alloc> &a,
						    size_t cursorAxis1,
						    size_t cursorAxis2)
: ArrayIterator<T, Alloc>(a, IPosition(2, cursorAxis1, cursorAxis2), true)
{
    // We need to ensure that ap points at a Matrix
    this->ap_p.reset( new Matrix<T, Alloc>(*this->ap_p) );  // reference
}

} //# NAMESPACE CASACORE - END

#endif
