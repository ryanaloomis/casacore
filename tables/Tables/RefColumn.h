//# RefColumn.h: A column in a reference table
//# Copyright (C) 1994,1995,1996,1997,1998
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

#ifndef TABLES_REFCOLUMN_H
#define TABLES_REFCOLUMN_H


//# Includes
#include <casacore/casa/aips.h>
#include <casacore/tables/Tables/BaseColumn.h>
#include <casacore/tables/Tables/ColumnCache.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore { //# NAMESPACE CASACORE - BEGIN

//# Forward Declarations
class RefTable;
class BaseColumnDesc;
class TableRecord;
class Slicer;
class IPosition;


// <summary>
// A column in a reference table
// </summary>

// <use visibility=local>

// <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> RefTable
//   <li> BaseColumn
// </prerequisite>

// <etymology>
// RefTable represents a column in a RefTable. A RefTable is a table
// referencing another table, usually as the result of a select, etc..
// </etymology>

// <synopsis> 
// RefColumn handles the access of a column in a RefTable.
// It calls the corresponding function in the referenced column
// while converting the given row number to the row number in the
// referenced table.
// </synopsis> 

// <motivation>
// This class is untyped, i.e. not templated.
// Every call is sent to the underlying referenced BaseColumn which
// is typed by the virtual function mechanism.
// A RefColumn can never be used directly. A user always has to
// construct a typed ArrayColumn or ScalarColumn object to access a column.
// This means everyting is fully type safe.
// </motivation>

// <todo asof="$DATE:$">
//# A List of bugs, limitations, extensions or planned refinements.
//   <li> Act upon removal of rows or the underlying column
// </todo>


class RefColumn : public BaseColumn
{
public:

    // Construct the RefColumn. It will point to the given column
    // description, RefTable and referenced column.
    // The RefTable will be used to convert the rownr to the rownr
    // in the referenced column.
    RefColumn (const BaseColumnDesc*, RefTable*, BaseColumn* referencedColumn);

    ~RefColumn();

    // Test if the column is writable in the parent table.
    virtual Bool isWritable() const;

    // Test if the column is stored (otherwise it is virtual).
    virtual Bool isStored() const;

    // Get access to the column keyword set.
    // This is the keyword set in the referenced column.
    // <group>
    virtual TableRecord& rwKeywordSet();
    virtual TableRecord& keywordSet();
    // </group>

    // Get nr of rows in the column.
    virtual rownr_t nrow() const;

    // Test if a value in a particular cell has been defined.
    virtual Bool isDefined (rownr_t rownr) const;

    // Set the shape of the array in the given row.
    virtual void setShape (rownr_t rownr, const IPosition& shape);

    // Set the shape and tile shape of the array in the given row.
    virtual void setShape (rownr_t rownr, const IPosition& shape,
			   const IPosition& tileShape);

    // Get the global #dimensions of an array (i.e. for all rows).
    virtual uInt ndimColumn() const;

    // Get the global shape of an array (i.e. for all rows).
    virtual IPosition shapeColumn() const;

    // Get the #dimensions of an array in a particular cell.
    virtual uInt ndim (rownr_t rownr) const;

    // Get the shape of an array in a particular cell.
    virtual IPosition shape (rownr_t rownr) const;

    // It can change shape if the underlying column can.
    virtual Bool canChangeShape() const;

    // Initialize the rows from startRownr till endRownr (inclusive)
    // with the default value defined in the column description (if defined).
    void initialize (rownr_t startRownr, rownr_t endRownr);

    // Get the value from a particular cell.
    // This can be a scalar or an array.
    virtual void get (rownr_t rownr, void* dataPtr) const;

    // Get an array from a particular cell.
    virtual void getArray (rownr_t rownr, ArrayBase& dataPtr) const;

    // Get a slice of an N-dimensional array in a particular cell.
    virtual void getSlice (rownr_t rownr, const Slicer&, ArrayBase& dataPtr) const;

    // Get the vector of all scalar values in a column.
    virtual void getScalarColumn (ArrayBase& dataPtr) const;

    // Get the array of all array values in a column.
    // If the column contains n-dim arrays, the resulting array is (n+1)-dim.
    // The arrays in the column have to have the same shape in all cells.
    virtual void getArrayColumn (ArrayBase& dataPtr) const;

    // Get subsections from all arrays in the column.
    // If the column contains n-dim arrays, the resulting array is (n+1)-dim.
    // The arrays in the column have to have the same shape in all cells.
    virtual void getColumnSlice (const Slicer&, ArrayBase& dataPtr) const;

    // Get the vector of some scalar values in a column.
    virtual void getScalarColumnCells (const RefRows& rownrs,
				       ArrayBase& dataPtr) const;

    // Get the array of some array values in a column.
    // If the column contains n-dim arrays, the resulting array is (n+1)-dim.
    // The arrays in the column have to have the same shape in all cells.
    virtual void getArrayColumnCells (const RefRows& rownrs,
				      ArrayBase& dataPtr) const;

    // Get subsections from some arrays in the column.
    // If the column contains n-dim arrays, the resulting array is (n+1)-dim.
    // The arrays in the column have to have the same shape in all cells.
    virtual void getColumnSliceCells (const RefRows& rownrs,
				      const Slicer&, ArrayBase& dataPtr) const;

    // Put the value in a particular cell.
    // This can be a scalar or an array.
    virtual void put (rownr_t rownr, const void* dataPtr);

    // Put the array value in a particular cell.
    virtual void putArray (rownr_t rownr, const ArrayBase& dataPtr);

    // Put a slice of an N-dimensional array in a particular cell.
    virtual void putSlice (rownr_t rownr, const Slicer&, const ArrayBase& dataPtr);

    // Put the vector of all scalar values in the column.
    virtual void putScalarColumn (const ArrayBase& dataPtr);

    // Put the array of all array values in the column.
    // If the column contains n-dim arrays, the source array is (n+1)-dim.
    // The arrays in the column have to have the same shape in all cells.
    virtual void putArrayColumn (const ArrayBase& dataPtr);

    // Put into subsections of all table arrays in the column.
    // If the column contains n-dim arrays, the source array is (n+1)-dim.
    // The arrays in the column have to have the same shape in all cells.
    virtual void putColumnSlice (const Slicer&, const ArrayBase& dataPtr);

    // Get the vector of some scalar values in a column.
    virtual void putScalarColumnCells (const RefRows& rownrs,
				       const ArrayBase& dataPtr);

    // Get the array of some array values in a column.
    // If the column contains n-dim arrays, the resulting array is (n+1)-dim.
    // The arrays in the column have to have the same shape in all cells.
    virtual void putArrayColumnCells (const RefRows& rownrs,
				      const ArrayBase& dataPtr);

    // Put subsections of some arrays in the column.
    // If the column contains n-dim arrays, the source array is (n+1)-dim.
    // The arrays in the column have to have the same shape in all cells.
    virtual void putColumnSliceCells (const RefRows& rownrs,
				      const Slicer&, const ArrayBase& dataPtr);

    // Get the underlying column cache.
    virtual ColumnCache& columnCache();

    // Set the maximum cache size (in bytes) to be used by a storage manager.
    virtual void setMaximumCacheSize (uInt nbytes);

    // Add this column and its data to the Sort object.
    // It may allocate some storage on the heap, which will be saved
    // in the argument dataSave.
    // The function freeSortKey must be called to free this storage.
    virtual void makeSortKey (Sort&, CountedPtr<BaseCompare>& cmpObj,
			      Int order, CountedPtr<ArrayBase>& dataSave);

    // Allocate value buffers for the table iterator.
    // Also get a comparison functiuon if undefined.
    // The function freeIterBuf must be called to free the buffers.
    virtual void allocIterBuf (void*& lastVal, void*& curVal,
			       CountedPtr<BaseCompare>& cmpObj);

    // Free the value buffers allocated by allocIterBuf.
    virtual void freeIterBuf (void*& lastVal, void*& curVal);

protected:
    RefTable*        refTabPtr_p;
    BaseColumn*      colPtr_p;
    ColumnCache      colCache_p;
};




} //# NAMESPACE CASACORE - END

#endif
