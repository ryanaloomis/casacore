//# MSHistoryColumns.h: provides easy access to MSHistory columns
//# Copyright (C) 1996,1999,2000
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

#ifndef MS_MSHISTORYCOLUMNS_H
#define MS_MSHISTORYCOLUMNS_H

#include <casacore/casa/aips.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <casacore/measures/Measures/MCEpoch.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/measures/TableMeasures/ScalarMeasColumn.h>
#include <casacore/measures/TableMeasures/ScalarQuantColumn.h>
#include <casacore/casa/BasicSL/String.h>

namespace casacore { //# NAMESPACE CASACORE - BEGIN

class MSHistory;

// <summary>
// A class to provide easy access to MSHistory columns
// </summary>

// <use visibility=export>

// <reviewed reviewer="Bob Garwood" date="1997/02/01" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MSHistory
//   <li> ArrayColumn
//   <li> ScalarColumn
// </prerequisite>
//
// <etymology>
// MSHistoryColumns stands for MeasurementSet History Table columns.
// </etymology>
//
// <synopsis>
// This class provides access to the columns in the MSHistory Table,
// it does the declaration of all the Scalar and ArrayColumns with the
// correct types, so the application programmer doesn't have to
// worry about getting those right. There is an access function
// for every predefined column. Access to non-predefined columns will still
// have to be done with explicit declarations.
// See <linkto class=MSColumns> MSColumns</linkto> for an example.
//
// Note by GvD 28-Jan-2010:
// According to note 229 the OBJECTID column should contain Strings.
// It is, however, defined as Int. It has to be left as such, otherwise older
// MeasurementSets cannot be read anymore.
// </synopsis>
//
// <motivation>
// See <linkto class=MSColumns> MSColumns</linkto> for the motivation.
// </motivation>

class MSHistoryColumns
{
public:
  // Create a columns object that accesses the data in the specified Table
  MSHistoryColumns(const MSHistory& msHistory);

  // The destructor does nothing special
  ~MSHistoryColumns();

  // Access to required columns
  // <group>
  ScalarColumn<String>& application() {return application_p;}
  ArrayColumn<String>& appParams() {return appParams_p;}
  ArrayColumn<String>& cliCommand() {return cliCommand_p;}
  ScalarColumn<String>& message() {return message_p;}
  ScalarColumn<Int>& objectId() {return objectId_p;}
  ScalarColumn<Int>& observationId() {return observationId_p;}
  ScalarColumn<String>& origin() {return origin_p;}
  ScalarColumn<String>& priority() {return priority_p;}
  ScalarColumn<Double>& time() {return time_p;}
  ScalarQuantColumn<Double>& timeQuant() {return timeQuant_p;}
  ScalarMeasColumn<MEpoch>& timeMeas() {return timeMeas_p;}
  // </group>

  // Const access to required columns
  // <group>
  const ScalarColumn<String>& application() const {return application_p;}
  const ArrayColumn<String>& appParams() const {return appParams_p;}
  const ArrayColumn<String>& cliCommand() const {return cliCommand_p;}
  const ScalarColumn<String>& message() const {return message_p;}
  const ScalarColumn<Int>& objectId() const {return objectId_p;}
  const ScalarColumn<Int>& observationId() const {return observationId_p;}
  const ScalarColumn<String>& origin() const {return origin_p;}
  const ScalarColumn<String>& priority() const {return priority_p;}
  const ScalarColumn<Double>& time() const {return time_p;}
  const ScalarQuantColumn<Double>& timeQuant() const {return timeQuant_p;}
  const ScalarMeasColumn<MEpoch>& timeMeas() const {return timeMeas_p;}
  // </group>

  // Convenience function that returns the number of rows in any of the columns
  rownr_t nrow() const {return application_p.nrow();}

  // set the epoch type for the TIME column.
  // <note role=tip>
  // In principle this function can only be used if the table is empty,
  // otherwise already written values may thereafter have an incorrect
  // reference, offset, or unit.  However, it is possible that part of the
  // table gets written before these values are known.  In that case the
  // reference, offset, or units can be set by using a False
  // <src>tableMustBeEmpty</src> argument.
  // </note>
  void setEpochRef(MEpoch::Types ref, Bool tableMustBeEmpty=True);

protected:
  //# default constructor creates a object that is not usable. Use the attach
  //# function correct this.
  MSHistoryColumns();

  //# attach this object to the supplied table.
  void attach(const MSHistory& msHistory);

private:
  //# Make the assignment operator and the copy constructor private to prevent
  //# any compiler generated one from being used.
  MSHistoryColumns(const MSHistoryColumns&);
  MSHistoryColumns& operator=(const MSHistoryColumns&);

  //# required columns
  ScalarColumn<String> application_p;
  ArrayColumn<String> appParams_p;
  ArrayColumn<String> cliCommand_p;
  ScalarColumn<String> message_p;
  ScalarColumn<Int> objectId_p;
  ScalarColumn<Int> observationId_p;
  ScalarColumn<String> origin_p;
  ScalarColumn<String> priority_p;
  ScalarColumn<Double> time_p;

  //# Access to Measure columns
  ScalarMeasColumn<MEpoch> timeMeas_p;

  //# Access to Quantum columns
  ScalarQuantColumn<Double> timeQuant_p;
};

//# Define the RO version for backward compatibility.
typedef MSHistoryColumns ROMSHistoryColumns;

} //# NAMESPACE CASACORE - END

#endif
