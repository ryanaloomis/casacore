//# TempLatticeImpl.cc: A Lattice that can be used for temporary storage
//# Copyright (C) 1997,1998,1999,2000,2001,2003
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

#ifndef LATTICES_TEMPLATTICEIMPL_TCC
#define LATTICES_TEMPLATTICEIMPL_TCC

#include <casacore/lattices/Lattices/TempLatticeImpl.h>
#include <casacore/lattices/Lattices/PagedArray.h>
#include <casacore/lattices/Lattices/ArrayLattice.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/SetupNewTab.h>
#include <casacore/tables/Tables/TableDesc.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/System/AppInfo.h>
#include <casacore/casa/OS/HostInfo.h>


namespace casacore { //# NAMESPACE CASACORE - BEGIN

template<class T>
TempLatticeImpl<T>::TempLatticeImpl() 
: itsTablePtr (0),
  itsIsClosed (False)
{
  itsLatticePtr = new ArrayLattice<T>;
}

template<class T>
TempLatticeImpl<T>::TempLatticeImpl (const TiledShape& shape, Int maxMemoryInMB)
: itsTablePtr (0),
  itsIsClosed (False)
{
  init (shape, Double(maxMemoryInMB));
}

template<class T>
TempLatticeImpl<T>::TempLatticeImpl (const TiledShape& shape, Double maxMemoryInMB)
: itsTablePtr (0),
  itsIsClosed (False)
{
  init(shape, maxMemoryInMB);
}

template<class T>
TempLatticeImpl<T>::~TempLatticeImpl()
{
  // Reopen to make sure that temporary table gets deleted.
  doReopen();
  delete itsTablePtr;
}

template<class T>
void TempLatticeImpl<T>::init (const TiledShape& shape, Double maxMemoryInMB) 
{
  Double memoryReq = Double(shape.shape().product()*sizeof(T))/(1024.0*1024.0);
  Double memoryAvail;
  // maxMemoryInMb = 0.0 forces disk.
  if (maxMemoryInMB < 0.0) {
    memoryAvail = Double(HostInfo::memoryFree()/1024) / 2.0;
  } else {
    memoryAvail = maxMemoryInMB;
  }
  if (memoryReq > memoryAvail) {
    // Create a table with a unique name in a work directory.
    // We can use exclusive locking, since nobody else should use the table.
    itsTableName = AppInfo::workFileName (Int(memoryReq), "TempLattice");
    SetupNewTable newtab (itsTableName, TableDesc(), Table::Scratch);
    itsTablePtr = new Table (newtab, TableLock::PermanentLockingWait);
    itsLatticePtr = new PagedArray<T> (shape, *itsTablePtr);
  } else {
    itsLatticePtr = new ArrayLattice<T> (shape.shape());
  }
}

template<class T>
void TempLatticeImpl<T>::tempClose()
{
  if (itsTablePtr != 0 && isPaged()) {
    // Take care that table does not get deleted, otherwise we cannot reopen.
    itsTablePtr->unmarkForDelete();
    delete itsTablePtr;
    itsTablePtr = 0;
    itsLatticePtr = 0;           // CountedPtr does delete of pointer
    itsIsClosed = True;
  }
}

template<class T>
void TempLatticeImpl<T>::tempReopen() const
{
  if (itsIsClosed && isPaged()) {
    itsTablePtr = new Table (itsTableName,
			     TableLock(TableLock::PermanentLockingWait),
			     Table::Update);
    itsLatticePtr = new PagedArray<T> (*itsTablePtr);
    itsIsClosed = False;
  }
  if (itsTablePtr != 0) {
    itsTablePtr->markForDelete();
  }
}

} //# NAMESPACE CASACORE - END


#endif
