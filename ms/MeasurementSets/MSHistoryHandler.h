//# MSHistoryHandler: allow simple access to write or read HISTORY subtable
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify
//# it under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or
//# (at your option) any later version.
//#
//# This program is distributed in the hope that it will be useful,
//# but WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//# GNU General Public License for more details.
//#
//# You should have received a copy of the GNU General Public License
//# along with this program; if not, write to the Free Software
//# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA


#ifndef MS_MSHISTORYHANDLER_H
#define MS_MSHISTORYHANDLER_H

#include <casacore/casa/aips.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSHistory.h>


namespace casacore { //# NAMESPACE CASACORE - BEGIN

class MSHistoryColumns;
class LogIO;
class LogSinkInterface;
// <summary>
// A class to provide a simple interface to history writing
// </summary>
// <use visibility=local>
// <etymology>
// Handle the history info that needs to be archived in ms
// </etymology>
// <synopsis>
// This class provides access to the MS history via single method calls
// One of the methods is static and can be called on a MeasurementSet without
// constructing any MSHistoryHandler objects.
// </synopsis>

class MSHistoryHandler
{

 public: 
  //Construct the history handler from an ms
  MSHistoryHandler(MeasurementSet& ms, const String &app="");

  MSHistoryHandler &operator=(const MSHistoryHandler &other);
  // Destructor
  ~MSHistoryHandler();

  

  //Add a string message

  // This method does not need construction ...can be called explicitly 
  // it flushes the history table of the ms
  static void addMessage(MeasurementSet& ms, const String& message,
	     const String& app="",
	     const String& cliComm="",
	     const String& origin="");

  // Add message and/or CLI command to the history table. It does not flush the table (the
  // destructor will flush).
  void addMessage(const String& message, const String& cliComm="", const String& origin="");
  // In this version the LogIO object need to have a valid LogSink with 
  // messages in it. 
  void addMessage(LogIO& message, const String& cliComm="");
  void addMessage(LogSinkInterface& sink, const String& cliComm="");

  void cliCommand(const String& cliComm);
  void cliCommand(LogIO& cliComm);
  void cliCommand(LogSinkInterface& sink);

 private: 

  // Prevent use of default constructor
  MSHistoryHandler() {}

  MSHistoryColumns *msHistCol_p;
  MSHistory  histTable_p;
  String application_p;

};


} //# NAMESPACE CASACORE - END

#endif
