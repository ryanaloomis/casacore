//# MaskedArray.cc: A templated N-D masked array class with variable origin.
//# Copyright (C) 1993,1994,1995,1996,1997,1999,2001,2005
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

#ifndef CASA_MASKEDARRAY_2_TCC
#define CASA_MASKEDARRAY_2_TCC

#include "MaskedArray.h"
#include "Array.h"
#include "ArrayLogical.h"
#include "Slicer.h"
#include "ArrayError.h"

#include <cassert>

namespace casacore {                                        //# NAMESPACE CASACORE - BEGIN

template<typename T, typename ArrayAlloc, typename MaskAlloc> 
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray () :
pArray (),
pMask (),
nelemValid (0),
nelemValidIsOK (false),
isRO (false)
{}

template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const array_type &inarray,
  const LogicalArray &inmask,
  bool isreadonly)
: pArray (), pMask (), nelemValid (0), nelemValidIsOK (false),
isRO (isreadonly)
{
  //    if (! conform2 (inarray, inmask)) {
  if (inarray.shape() != inmask.shape()) {
    throw (ArrayConformanceError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray(const array_type &,"
      " const LogicalArray &, bool)"
      " - arrays do not conform"));
   }

  pArray.reset( new array_type (inarray) );
  pMask.reset( new LogicalArray (inmask.shape()) );
  pMask->assign_conforming(inmask);

  assert(ok());
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const array_type &inarray,
  const LogicalArray &inmask)
: pArray(), pMask (), nelemValid (0), nelemValidIsOK (false), isRO (false)
{
  //    if (! conform2 (inarray, inmask)) {
  if (inarray.shape() !=  inmask.shape()) {
    throw (ArrayConformanceError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray(const array_type &,"
      " const LogicalArray &)"
      " - arrays do not conform"));
   }

  pArray.reset( new array_type (inarray) );
  pMask.reset( new LogicalArray (inmask.shape()) );
  pMask->assign_conforming(inmask);

  assert(ok());

}

template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const MaskedArray<T, ArrayAlloc, MaskAlloc> &inarray,
  const LogicalArray &inmask,
  bool isreadonly)
: pArray (), pMask (), nelemValid (0), nelemValidIsOK (false),
isRO ( (inarray.isRO || isreadonly))
{
  //    if (! conform2 (inarray, inmask)) {
  if (inarray.shape() != inmask.shape()) {
    throw (ArrayConformanceError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const MaskedArray<T, ArrayAlloc, MaskAlloc> &,"
      " const LogicalArray &, bool)"
      " - arrays do not conform"));
   }


  pArray.reset( new array_type (inarray.getArray()) );
  pMask.reset( new LogicalArray (inmask.shape()) );
  *pMask = (inmask && inarray.getMask());

  assert(ok());

}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const MaskedArray<T, ArrayAlloc, MaskAlloc> &inarray,
const LogicalArray &inmask)
: pArray (), pMask (), nelemValid (0), nelemValidIsOK (false),
isRO (inarray.isRO)
{
  //    if (! conform2 (inarray, inmask)) {
  if (inarray.shape() != inmask.shape()) {
    throw (ArrayConformanceError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const MaskedArray<T, ArrayAlloc, MaskAlloc> &,"
      " const LogicalArray &)"
      " - arrays do not conform"));
   }


  pArray.reset( new array_type (inarray.getArray()) );
  pMask.reset( new LogicalArray (inmask.shape()) );
  *pMask = (inmask && inarray.getMask());

  assert(ok());

}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const array_type &inarray,
const MaskedLogicalArray &inmask,
bool isreadonly)
: pArray (), pMask (), nelemValid (0), nelemValidIsOK (false),
isRO (isreadonly)
{
  //    if (! conform2 (inarray, inmask)) {
  if (inarray.shape() != inmask.shape()) {
    throw (ArrayConformanceError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray(const array_type &inarray,"
      " const MaskedLogicalArray &inmask, bool isreadonly)"
      " - arrays do not conform"));
   }

  pArray.reset( new array_type (inarray) );
  pMask.reset( new LogicalArray (inarray.shape()) );
  pMask->assign_conforming(inmask.getArray() && inmask.getMask());

  assert(ok());

}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const array_type &inarray,
const MaskedLogicalArray &inmask)
: pArray (), pMask (), nelemValid (0), nelemValidIsOK (false), isRO (false)
{
  //    if (! conform2 (inarray, inmask)) {
  if (inarray.shape() != inmask.shape()) {
    throw (ArrayConformanceError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray(const array_type &inarray,"
      " const MaskedLogicalArray &inmask)"
      " - arrays do not conform"));
   }

  pArray.reset( new array_type (inarray) );
  pMask.reset( new LogicalArray (inarray.shape()) );
  pMask->assign_conforming(inmask.getArray() && inmask.getMask());

  assert(ok());

}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const MaskedArray<T, ArrayAlloc, MaskAlloc> &inarray,
const MaskedLogicalArray &inmask)
: pArray (), pMask (), nelemValid (0), nelemValidIsOK (false),
isRO (inarray.isRO)
{
  //    if (! conform2 (inarray, inmask)) {
  if (inarray.shape() != inmask.shape()) {
    throw (ArrayConformanceError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const MaskedArray<T, ArrayAlloc, MaskAlloc> &inarray,"
      " const MaskedLogicalArray &inmask)"
      " - arrays do not conform"));
   }


  pArray.reset( new array_type (inarray.getArray()) );
  pMask.reset( new LogicalArray (inarray.shape()) );
  *pMask = (inmask.getArray() && inmask.getMask() && inarray.getMask());

  assert(ok());

}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const MaskedArray<T, ArrayAlloc, MaskAlloc> &inarray,
const MaskedLogicalArray &inmask,
bool isreadonly)
: pArray (), pMask (), nelemValid (0), nelemValidIsOK (false),
isRO ( (inarray.isRO || isreadonly))
{
  //    if (! conform2 (inarray, inmask)) {
  if (inarray.shape() != inmask.shape()) {
    throw (ArrayConformanceError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray (const MaskedArray<T, ArrayAlloc, MaskAlloc> &inarray,"
      " const MaskedLogicalArray &inmask, bool isreadonly)"
      " - arrays do not conform"));
   }


  pArray.reset( new array_type (inarray.getArray()) );
  pMask.reset( new LogicalArray (inarray.shape()) );
  *pMask = (inmask.getArray() && inmask.getMask() && inarray.getMask());

  assert(ok());

}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray(const MaskedArray<T, ArrayAlloc, MaskAlloc> &other, bool isreadonly)
: pArray (), pMask (),
nelemValid (other.nelemValid), nelemValidIsOK (other.nelemValidIsOK),
isRO ( (other.isRO || isreadonly))
{

  pArray.reset( new array_type (*(other.pArray)) );
  pMask.reset( new LogicalArray (*(other.pMask)) );

  assert(ok());

}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray(const MaskedArray<T, ArrayAlloc, MaskAlloc> &other)
: pArray (), pMask (),
nelemValid (other.nelemValid), nelemValidIsOK (other.nelemValidIsOK),
isRO (other.isRO)
{

  pArray.reset( new array_type (*(other.pArray)) );
  pMask.reset( new LogicalArray (*(other.pMask)) );

  assert(ok());

}

template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>::MaskedArray(MaskedArray<T, ArrayAlloc, MaskAlloc>&& source)
: pArray (std::move(source.pArray)),
pMask (std::move(source.pMask)),
nelemValid (source.nelemValid),
nelemValidIsOK (source.nelemValidIsOK),
isRO (source.isRO)
{
  source.nelemValid = 0;
  source.nelemValidIsOK = false;
  source.isRO = false;
  assert(ok());
  assert(source.ok());
}

template<typename T, typename ArrayAlloc, typename MaskAlloc> void
MaskedArray<T, ArrayAlloc, MaskAlloc>::setData (const array_type &data,
const mask_type &mask,
bool isReadOnly) {
  if (data.shape() != mask.shape()) {
    throw (ArrayConformanceError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc>::setData(const array_type &,"
      " const LogicalArray &, bool)"
      " - arrays do not conform"));
   }
  pArray.reset( new array_type(data) );
  pMask.reset( new mask_type (mask.copy()) );
  nelemValid = 0;
  nelemValidIsOK = false;
  isRO  = isReadOnly; 
  assert(ok());
}

template<typename T, typename ArrayAlloc, typename MaskAlloc> void
MaskedArray<T, ArrayAlloc, MaskAlloc>::setData (const MaskedArray<T, ArrayAlloc, MaskAlloc> & array,
bool isReadOnly){
  pArray.reset( new array_type(array.getArray()) );
  pMask.reset( new LogicalArray(array.getMask().copy()) );
  nelemValid = 0;
  nelemValidIsOK = false;
  isRO  = isReadOnly; 

  assert(ok());
 }


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc> MaskedArray<T, ArrayAlloc, MaskAlloc>::copy(bool isreadonly) const
{
  assert(ok());

  MaskedArray<T, ArrayAlloc, MaskAlloc> retval (pArray->copy(), *pMask, isreadonly);
  retval.nelemValid = nelemValid;
  retval.nelemValidIsOK = nelemValidIsOK;

  return retval;
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc> MaskedArray<T, ArrayAlloc, MaskAlloc>::copy() const
{
  assert(ok());

  MaskedArray<T, ArrayAlloc, MaskAlloc> retval (pArray->copy(), *pMask);
  retval.nelemValid = nelemValid;
  retval.nelemValidIsOK = nelemValidIsOK;

  return retval;
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc> MaskedArray<T, ArrayAlloc, MaskAlloc>::operator() (const LogicalArray &mask) const
{
  assert(ok());

  MaskedArray<T, ArrayAlloc, MaskAlloc> ret (*this, mask);
  return ret;
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc> MaskedArray<T, ArrayAlloc, MaskAlloc>::operator()
(const MaskedLogicalArray &mask) const
{
  assert(ok());

  MaskedArray<T, ArrayAlloc, MaskAlloc> ret (*this, mask);
  return ret;
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc> MaskedArray<T, ArrayAlloc, MaskAlloc>::operator() (const IPosition &start,
const IPosition &end)
{
  assert(ok());
  return MaskedArray<T, ArrayAlloc, MaskAlloc> ((*pArray)(start,end), (*pMask)(start,end), isRO);
}

template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc> MaskedArray<T, ArrayAlloc, MaskAlloc>::operator() (const IPosition &start,
const IPosition &end,
const IPosition &inc)
{
  assert(ok());
  return MaskedArray<T, ArrayAlloc, MaskAlloc> ((*pArray)(start,end,inc), (*pMask)(start,end,inc),
    isRO);
}

template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc> MaskedArray<T, ArrayAlloc, MaskAlloc>::operator() (const Slicer &slicer)
{
  assert(ok());
  return MaskedArray<T, ArrayAlloc, MaskAlloc> ((*pArray)(slicer), (*pMask)(slicer), isRO);
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
const Array<T, ArrayAlloc> &MaskedArray<T, ArrayAlloc, MaskAlloc>::getArray() const
{
  assert(ok());

  return *pArray;
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
const Array<LogicalArrayElem, MaskAlloc> & MaskedArray<T, ArrayAlloc, MaskAlloc>::getMask() const
{
  assert(ok());

  return *pMask;
}


template<typename T, typename ArrayAlloc, typename MaskAlloc> size_t MaskedArray<T, ArrayAlloc, MaskAlloc>::ndim() const
{
  assert(ok());

  return pArray->ndim();
}


template<typename T, typename ArrayAlloc, typename MaskAlloc> size_t MaskedArray<T, ArrayAlloc, MaskAlloc>::nelementsValid() const
{
  assert(ok());

  if (!nelemValidIsOK) {
    // Calculate nelemValid;

    bool maskDelete;
    const LogicalArrayElem *maskStorage = getMaskStorage(maskDelete);
    const LogicalArrayElem *maskS = maskStorage;

    size_t nelemValidTmp = 0;
    size_t ntotal = nelements();
    while (ntotal--) {
      if (*maskS) {
        nelemValidTmp++;
       }
      maskS++;
     }

    freeMaskStorage(maskStorage, maskDelete);

    MaskedArray<T, ArrayAlloc, MaskAlloc> *nonconstThis = (MaskedArray<T, ArrayAlloc, MaskAlloc> *) this;
    nonconstThis->nelemValid = nelemValidTmp;
    nonconstThis->nelemValidIsOK = true;
   }

  return nelemValid;
}


template<typename T, typename ArrayAlloc, typename MaskAlloc> size_t MaskedArray<T, ArrayAlloc, MaskAlloc>::nelements() const
{
  assert(ok());

  return pArray->nelements();
}


template<typename T, typename ArrayAlloc, typename MaskAlloc> bool MaskedArray<T, ArrayAlloc, MaskAlloc>::ok() const
{
  if (!pArray && !pMask) return true;                       // default constructed is ok
  if (!pArray || !pMask) return false;                      // not both set is not ok
  return (pArray->ok() && pMask->ok()) ? true : false;
}


template<typename T, typename ArrayAlloc, typename MaskAlloc> bool MaskedArray<T, ArrayAlloc, MaskAlloc>::conform(const array_type &other) const
{
  assert(ok());

  return pArray->conform(other);
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
bool MaskedArray<T, ArrayAlloc, MaskAlloc>::conform(const MaskedArray<T, ArrayAlloc, MaskAlloc> &other) const
{
  assert(ok());

  return pArray->conform(*(other.pArray));
}


template<typename T, typename ArrayAlloc, typename MaskAlloc> void MaskedArray<T, ArrayAlloc, MaskAlloc>::setReadOnly() const
{
  assert(ok());

  MaskedArray<T, ArrayAlloc, MaskAlloc> *nonconstThis = (MaskedArray<T, ArrayAlloc, MaskAlloc> *) this;
  nonconstThis->isRO = true;
}


template <typename T, typename ArrayAlloc, typename MaskAlloc>
Array<T, ArrayAlloc> MaskedArray<T, ArrayAlloc, MaskAlloc>::getCompressedArray () const
{
  array_type result (IPosition (1,nelementsValid()));

  bool deleteResult;
  T *resultStorage = result.getStorage (deleteResult);
  T *resultS = resultStorage;

  bool deleteArr;
  const T *arrStorage = getArrayStorage (deleteArr);
  const T *arrS = arrStorage;

  bool deleteMask;
  const LogicalArrayElem *maskStorage
  = getMaskStorage (deleteMask);
  const LogicalArrayElem *maskS = maskStorage;

  size_t ntotal = nelementsValid();
  while (ntotal) {
    if (*maskS) {
      *resultS = *arrS;
      resultS++;
      ntotal--;
     }
    maskS++;
    arrS++;
   }

  result.putStorage (resultStorage, deleteResult);
  freeArrayStorage (arrStorage, deleteArr);
  freeMaskStorage (maskStorage, deleteMask);

  return result;
}


template <typename T, typename ArrayAlloc, typename MaskAlloc>
Array<T, ArrayAlloc> MaskedArray<T, ArrayAlloc, MaskAlloc>::getCompressedArray (const IPosition & shape) const
{
  if (int(nelementsValid()) != shape.product()) {
    throw (ArrayError
      ("void MaskedArray<T, ArrayAlloc, MaskAlloc>::getCompressedArray (const IPosition & shape)"
        " - input shape will create Array with incorrect number of elements"));

   }

  array_type result (shape);

  bool deleteResult;
  T *resultStorage = result.getStorage (deleteResult);
  T *resultS = resultStorage;

  bool deleteArr;
  const T *arrStorage = getArrayStorage (deleteArr);
  const T *arrS = arrStorage;

  bool deleteMask;
  const LogicalArrayElem *maskStorage
  = getMaskStorage (deleteMask);
  const LogicalArrayElem *maskS = maskStorage;

  size_t ntotal = nelementsValid();
  while (ntotal) {
    if (*maskS) {
      *resultS = *arrS;
      resultS++;
      ntotal--;
     }
    maskS++;
    arrS++;
   }

  result.putStorage (resultStorage, deleteResult);
  freeArrayStorage (arrStorage, deleteArr);
  freeMaskStorage (maskStorage, deleteMask);

  return result;
}


template <typename T, typename ArrayAlloc, typename MaskAlloc>
void MaskedArray<T, ArrayAlloc, MaskAlloc>::getCompressedArray (array_type & inarr) const
{
  if (nelementsValid() != inarr.nelements()) {
    throw (ArrayError
      ("void MaskedArray<T, ArrayAlloc, MaskAlloc>::getCompressedArray (array_type & inarr)"
        " - input Array number of elements is incorrect"));

   }

  bool deleteInarr;
  T *inarrStorage = inarr.getStorage (deleteInarr);
  T *inarrS = inarrStorage;

  bool deleteArr;
  const T *arrStorage = getArrayStorage (deleteArr);
  const T *arrS = arrStorage;

  bool deleteMask;
  const LogicalArrayElem *maskStorage
  = getMaskStorage (deleteMask);
  const LogicalArrayElem *maskS = maskStorage;

  size_t ntotal = nelementsValid();
  while (ntotal) {
    if (*maskS) {
      *inarrS = *arrS;
      inarrS++;
      ntotal--;
     }
    maskS++;
    arrS++;
   }

  inarr.putStorage (inarrStorage, deleteInarr);
  freeArrayStorage (arrStorage, deleteArr);
  freeMaskStorage (maskStorage, deleteMask);
}


template <typename T, typename ArrayAlloc, typename MaskAlloc>
void MaskedArray<T, ArrayAlloc, MaskAlloc>::setCompressedArray (const array_type & inarr)
{
  if (nelementsValid() != inarr.nelements()) {
    throw (ArrayError
      ("void MaskedArray<T, ArrayAlloc, MaskAlloc>::setCompressedArray (const array_type & inarr)"
        " - input array number of elements is incorrect"));

   }

  bool deleteInarr;
  const T *inarrStorage = inarr.getStorage (deleteInarr);
  const T *inarrS = inarrStorage;

  bool deleteArr;
  T *arrStorage = getRWArrayStorage (deleteArr);
  T *arrS = arrStorage;

  bool deleteMask;
  const LogicalArrayElem *maskStorage
  = getMaskStorage (deleteMask);
  const LogicalArrayElem *maskS = maskStorage;

  size_t ntotal = nelementsValid();
  while (ntotal) {
    if (*maskS) {
      *arrS = *inarrS;
      inarrS++;
      ntotal--;
     }
    maskS++;
    arrS++;
   }

  inarr.freeStorage (inarrStorage, deleteInarr);
  putArrayStorage (arrStorage, deleteArr);
  freeMaskStorage (maskStorage, deleteMask);
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
const T * MaskedArray<T, ArrayAlloc, MaskAlloc>::getArrayStorage (bool &deleteIt) const
{
  assert(ok());

  return pArray->getStorage (deleteIt);
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
void MaskedArray<T, ArrayAlloc, MaskAlloc>::freeArrayStorage(const T *&storage, bool deleteIt) const
{
  assert(ok());

  pArray->freeStorage (storage, deleteIt);
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
const LogicalArrayElem * MaskedArray<T, ArrayAlloc, MaskAlloc>::getMaskStorage (bool &deleteIt) const
{
  assert(ok());

  return pMask->getStorage (deleteIt);
}


template<typename T, typename ArrayAlloc, typename MaskAlloc> void  MaskedArray<T, ArrayAlloc, MaskAlloc>::freeMaskStorage
(const LogicalArrayElem *&storage, bool deleteIt) const
{
  assert(ok());

  pMask->freeStorage (storage, deleteIt);
}



template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>& MaskedArray<T, ArrayAlloc, MaskAlloc>::operator=
  (const array_type &inarray)
{
  assert(ok());

  if (!pArray) {
    pArray.reset( new array_type(inarray) );
    pMask.reset( new mask_type(inarray.shape(), true) );
    nelemValid = 0;
    nelemValidIsOK = false;
    isRO  = false;
    
    return *this;
  }

  if (!conform(inarray)) {
    throw(ArrayConformanceError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc> & MaskedArray<T, ArrayAlloc, MaskAlloc>::operator= "
      "(const array_type &inarray)"
      "- Conformance error."));
   }

  if (isRO) {
    throw(ArrayError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc> & MaskedArray<T, ArrayAlloc, MaskAlloc>::operator= "
      "(const array_type &inarray)"
      "- this is read only."));
   }

  bool deleteArr;
  T *arrStorage = getRWArrayStorage(deleteArr);
  T *arrS = arrStorage;

  bool deleteMask;
  const LogicalArrayElem *maskStorage = getMaskStorage(deleteMask);
  const LogicalArrayElem *maskS = maskStorage;

  bool deleteInarr;
  const T *inarrStorage = inarray.getStorage(deleteInarr);
  const T *inarrS = inarrStorage;

  size_t ntotal = pArray->nelements();
  while (ntotal--) {
    if (*maskS) {
      *arrS = *inarrS;
     }
    arrS++;
    maskS++;
    inarrS++;
   }

  putArrayStorage(arrStorage, deleteArr);
  freeMaskStorage(maskStorage, deleteMask);
  inarray.freeStorage(inarrStorage, deleteInarr);

  return *this;
}

template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>& MaskedArray<T, ArrayAlloc, MaskAlloc>::operator=
  (array_type&& inarray)
{
  assert(ok());

  if (!pArray) {
    pMask.reset( new mask_type(inarray.shape(), true) );
    pArray.reset( new array_type(inarray) );
    nelemValid = 0;
    nelemValidIsOK = false;
    isRO  = false;
    return *this;
  }
  else {
    return operator=(inarray); // do ordinary copy assignment
  }
}

template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>& MaskedArray<T, ArrayAlloc, MaskAlloc>::operator=
  (const MaskedArray<T, ArrayAlloc, MaskAlloc> &other)
{
  assert(ok());

  if (this == &other)
  return *this;

  if (!pArray) {
    setData(other.copy());
    return *this;
   }

  if (!conform(other)) {
    throw(ArrayConformanceError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc> & MaskedArray<T, ArrayAlloc, MaskAlloc>::operator= "
      "(const MaskedArray<T, ArrayAlloc, MaskAlloc> &other)"
      "- Conformance error."));
   }

  if (isRO) {
    throw(ArrayError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc> & MaskedArray<T, ArrayAlloc, MaskAlloc>::operator= "
      "(const MaskedArray<T, ArrayAlloc, MaskAlloc> &other)"
      "- this is read only."));
   }

  bool deleteArr;
  T *arrStorage = getRWArrayStorage(deleteArr);
  T *arrS = arrStorage;

  bool deleteMask;
  const LogicalArrayElem *maskStorage = getMaskStorage(deleteMask);
  const LogicalArrayElem *maskS = maskStorage;

  bool deleteOarr;
  const T *oarrStorage = other.getArrayStorage(deleteOarr);
  const T *oarrS = oarrStorage;

  bool deleteOmask;
  const LogicalArrayElem *omaskStorage =
  other.getMaskStorage(deleteOmask);
  const LogicalArrayElem *omaskS = omaskStorage;

  size_t ntotal = pArray->nelements();
  while (ntotal--) {
    if (*maskS && *omaskS) {
      *arrS = *oarrS;
     }
    arrS++;
    maskS++;
    oarrS++;
    omaskS++;
   }

  putArrayStorage(arrStorage, deleteArr);
  freeMaskStorage(maskStorage, deleteMask);
  other.freeArrayStorage(oarrStorage, deleteOarr);
  other.freeMaskStorage(omaskStorage, deleteOmask);

  return *this;
}

template<typename T, typename ArrayAlloc, typename MaskAlloc>
MaskedArray<T, ArrayAlloc, MaskAlloc>& MaskedArray<T, ArrayAlloc, MaskAlloc>::operator=
  (MaskedArray<T, ArrayAlloc, MaskAlloc>&& other)
{
  assert(ok());

  if (other.isReadOnly())
    return operator=(other);
  
  if (this == &other)
    return *this;

  if (!pArray) {
    pArray = std::move(other.pArray);
    pMask = std::move(other.pMask);
    nelemValid = 0;
    nelemValidIsOK = false;
    isRO  = false;
    other.nelemValid = 0;
    other.nelemValidIsOK = false;
    // other.isRO = false; // we already know this is false
    return *this;
  }
  else {
    return operator=(other);
  }
}

template<typename T, typename ArrayAlloc, typename MaskAlloc> MaskedArray<T, ArrayAlloc, MaskAlloc> &MaskedArray<T, ArrayAlloc, MaskAlloc>::operator=(const T &val)
{
  assert(ok());
  if (!pArray) return *this;

  if (isRO) {
    throw(ArrayError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc> & MaskedArray<T, ArrayAlloc, MaskAlloc>::operator= (const T &val)"
      "- this is read only."));
   }

  bool deleteArr;
  T *arrStorage = getRWArrayStorage(deleteArr);
  T *arrS = arrStorage;

  bool deleteMask;
  const LogicalArrayElem *maskStorage = getMaskStorage(deleteMask);
  const LogicalArrayElem *maskS = maskStorage;

  size_t ntotal = pArray->nelements();
  while (ntotal--) {
    if (*maskS) {
      *arrS = val;
     }
    arrS++;
    maskS++;
   }

  putArrayStorage(arrStorage, deleteArr);
  freeMaskStorage(maskStorage, deleteMask);

  return *this;
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
T * MaskedArray<T, ArrayAlloc, MaskAlloc>::getRWArrayStorage (bool &deleteIt) const
{
  assert(ok());

  if (isRO) {
    throw(ArrayError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc>::getRWArrayStorage (bool &deleteIt) const"
      "- this is read only."));
   }

  return pArray->getStorage (deleteIt);
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
void MaskedArray<T, ArrayAlloc, MaskAlloc>::putArrayStorage(T *&storage, bool deleteAndCopy) const
{
  assert(ok());

  if (isRO) {
    throw(ArrayError(
      "MaskedArray<T, ArrayAlloc, MaskAlloc>::putArrayStorage (bool deleteAndCopy) const"
      "- this is read only."));
   }

  pArray->putStorage (storage, deleteAndCopy);
}


template<typename T, typename ArrayAlloc, typename MaskAlloc>
Array<T, ArrayAlloc>& MaskedArray<T, ArrayAlloc, MaskAlloc>::getRWArray() const
{
  assert(ok());

  if (isRO) {
    throw(ArrayError(
      "array_type & MaskedArray<T, ArrayAlloc, MaskAlloc>::getRWArray () const"
      "- this is read only."));
   }

  return *pArray;
}


//# Global functions.

template<typename TL, typename ArrayAllocL, typename MaskAllocL, typename TR, typename ArrayAllocR>
bool conform2 (const MaskedArray<TL, ArrayAllocL, MaskAllocL> &left, const Array<TR, ArrayAllocR> &right)
{
  IPosition leftShape (left.shape());
  IPosition rightShape (right.shape());

  return ( (leftShape.conform (rightShape)) && (leftShape == rightShape) )
 ? true : false;
}

template<typename TL, typename ArrayAllocL, typename TR, typename ArrayAllocR, typename MaskAllocR>
bool conform2 (const Array<TL, ArrayAllocL> &left, const MaskedArray<TR, ArrayAllocR, MaskAllocR> &right)
{
  IPosition leftShape (left.shape());
  IPosition rightShape (right.shape());

  return ( (leftShape.conform (rightShape)) && (leftShape == rightShape) )
 ? true : false;
}

template<typename TL, typename ArrayAllocL, typename MaskAllocL, typename TR, typename ArrayAllocR, typename MaskAllocR>
bool conform2 (const MaskedArray<TL, ArrayAllocL, MaskAllocL> &left, const MaskedArray<TR, ArrayAllocR, MaskAllocR> &right)
{
  IPosition leftShape (left.shape());
  IPosition rightShape (right.shape());

  return ( (leftShape.conform (rightShape)) && (leftShape == rightShape) )
 ? true : false;
}


}                                                         //# NAMESPACE CASACORE - END


#endif
