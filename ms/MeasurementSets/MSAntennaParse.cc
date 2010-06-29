//# MSAntennaParse.cc: Classes to hold results from antenna grammar parser
//# Copyright (C) 1994,1995,1997,1998,1999,2000,2001,2003
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
//#
//# $Id$

#include <ms/MeasurementSets/MSAntennaParse.h>
#include <ms/MeasurementSets/MSAntennaIndex.h>
#include <ms/MeasurementSets/MSSelectionError.h>
#include <ms/MeasurementSets/MSAntennaColumns.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/BasicSL/String.h>
#include <casa/Logging/LogIO.h>
#include <ms/MeasurementSets/MSSelectionTools.h>
namespace casa { //# NAMESPACE CASA - BEGIN

  MSAntennaParse* MSAntennaParse::thisMSAParser = 0x0; // Global pointer to the parser object
  TableExprNode MSAntennaParse::node_p;
  Vector<Int> MSAntennaParse::ant1List(0);
  Vector<Int> MSAntennaParse::ant2List(0);
  Matrix<Int> MSAntennaParse::baselineList(0,2);
  
  //# Constructor
  MSAntennaParse::MSAntennaParse ()
    : MSParse(),
      colName1(MS::columnName(MS::ANTENNA1)),
      colName2(MS::columnName(MS::ANTENNA2))
  {
  }
  
  //# Constructor with given ms name.
  MSAntennaParse::MSAntennaParse (const MeasurementSet* myms)
    : MSParse(myms, "Antenna"),
      colName1(MS::columnName(MS::ANTENNA1)),
      colName2(MS::columnName(MS::ANTENNA2))
  {
  }

  // Add the current condition to the TableExprNode tree.  Mask auto
  // correlations if autoCorr==False
  // 
  void MSAntennaParse::setTEN(TableExprNode& condition, 
			      BaselineListType autoCorr,
			      Bool negate)
  {
    if (autoCorr==CrossOnly) {
      TableExprNode noAutoCorr = (ms()->col(colName1) != ms()->col(colName2));
      condition = noAutoCorr && condition;
    }
    //    if (negate) cerr << "Generating a negation condition" << endl;
    if (negate) {
      node_p = node_p && !condition;
    } else {
      node_p = node_p || condition;
    }
  }

  const TableExprNode* MSAntennaParse::selectAntennaIds(const Vector<Int>& antennaIds, 
							BaselineListType autoCorr,
							Bool negate) 
  {
    TableExprNode condition;
    if ((autoCorr==AutoCorrAlso) || (autoCorr==AutoCorrOnly)) {
      Int n=antennaIds.nelements();
      if (n) {
        condition = ((ms()->col(colName1) == antennaIds[0]) &&
                     (ms()->col(colName2) == antennaIds[0]));
        for (Int i=1;i<n;i++) {
          condition = condition || 
            ((ms()->col(colName1) == antennaIds[i]) &&
             (ms()->col(colName2) == antennaIds[i]));
        }
      }
    } else {
      condition =
	(ms()->col(colName1).in(antennaIds) || ms()->col(colName2).in(antennaIds));
    }
    {
      Int nrows_p = ms()->antenna().nrow();
      Vector<Int> a2(nrows_p);
      a2.resize(nrows_p);
      indgen(a2);

      makeAntennaList(ant1List, antennaIds,negate);
      makeAntennaList(ant2List, a2);
      if (negate) {
        makeBaselineList(-antennaIds,a2,baselineList,autoCorr, negate);
      } else {
        makeBaselineList(antennaIds,a2,baselineList,autoCorr, negate);
      }
    }
    setTEN(condition,AutoCorrAlso , negate);
    return node();
  }

  void MSAntennaParse::makeAntennaList(Vector<Int>& antList,const Vector<Int>& thisList,
				       Bool negate)
  {
    Vector<Int> a2;
    if (negate) {
      a2=-thisList;
    } else {
      a2=thisList;
    }
    Vector<Int> tmp1(set_union(a2,antList));
    antList.resize(tmp1.nelements());antList = tmp1;
  }
  
  const TableExprNode* MSAntennaParse::selectAntennaIds(const Vector<Int>& antennaIds1,
							const Vector<Int>& antennaIds2,
							BaselineListType autoCorr,
							Bool negate)
  {
    TableExprNode condition;

    if (antennaIds2.size()) {
      condition =
	(ms()->col(colName1).in(antennaIds1)  && ms()->col(colName2).in(antennaIds2)) ||
	(ms()->col(colName1).in(antennaIds2)  && ms()->col(colName2).in(antennaIds1));
    } else {
      condition =
	(ms()->col(colName1).in(antennaIds1) && ms()->col(colName2).in(antennaIds1));
    }    
    makeAntennaList(ant1List, antennaIds1,negate);
    makeAntennaList(ant2List, antennaIds2,negate);

    if (negate) {
      makeBaselineList(-antennaIds1,-antennaIds2,baselineList,autoCorr, negate);
    } else {
      makeBaselineList(antennaIds1,antennaIds2,baselineList,autoCorr, negate);
    }
    setTEN(condition,autoCorr,negate);
    return node();
  }
  
  const TableExprNode* MSAntennaParse::selectNameOrStation(const Vector<String>& antenna, 
							   BaselineListType autoCorr,
							   Bool negate)
  {
    MSAntennaIndex msAI(ms()->antenna());
    
    Vector<Int> ant=msAI.matchAntennaName(antenna);

    TableExprNode condition =(ms()->col(colName1).in(ant) || ms()->col(colName2).in(ant));
    
    setTEN(condition,autoCorr,negate);
    return node();
  }
  
  const TableExprNode* MSAntennaParse::selectNameOrStation(const Vector<String>& antenna1,
							   const Vector<String>& antenna2,
							   BaselineListType autoCorr,
							   Bool negate)
  {
    MSAntennaIndex msAI(ms()->antenna());
    
    Vector<Int> a1=msAI.matchAntennaName(antenna1),
      a2 = msAI.matchAntennaName(antenna2);

    TableExprNode condition =
      (ms()->col(colName1).in(a1) && ms()->col(colName2).in(a2)) ||
      (ms()->col(colName1).in(a2) && ms()->col(colName2).in(a1));
    
    setTEN(condition,autoCorr,negate);
    return node();
  }
  
  const TableExprNode* MSAntennaParse::selectNameOrStation(const String& antenna1,
							   const String& antenna2,
							   BaselineListType autoCorr,
							   Bool negate)
  {
    TableExprNode condition =
      (ms()->col(colName1) >= antenna1 && ms()->col(colName2) <= antenna2) ||
      (ms()->col(colName2) >= antenna1 && ms()->col(colName1) <= antenna2);
    
    setTEN(condition,autoCorr,negate);
    return node();
  }
  
  const TableExprNode* MSAntennaParse::selectFromIdsAndCPs(const Int, const String&)
  {
    LogIO os(LogOrigin("MSAntennaParse", "selectFromIdsAndCPs()", WHERE));
    os << " selectFromIdsAndCPs is not available "  << LogIO::POST;
    
    TableExprNode condition;
    
    setTEN(condition);
    return node();
  }
  
  const TableExprNode* MSAntennaParse::selectFromIdsAndCPs(const Int, 
							   const String&, 
							   const Int, 
							   const String&)
  {
    LogIO os(LogOrigin("MSAntennaParse", "selectFromIdsAndCPs()", WHERE));
    os << " selectFromIdsAndCPs is not available "  << LogIO::POST;
    
    TableExprNode condition;
    
    setTEN(condition);
    return node();
  }
  
  const TableExprNode* MSAntennaParse::selectLength
  (const std::vector<double>& lengths, Bool negate)
  {
    TableExprNode selAnt1, selAnt2;
    Matrix<double> blength = getBaselineLengths();
    Matrix<Bool> match(blength.shape());
    match = False;
    int nr=0;
    for (Int j=0; j<blength.shape()[1]; ++j) {
      for (Int i=0; i<blength.shape()[0]; ++i) {
        double bl = blength(i,j);
        for (uInt k=0; k<lengths.size(); k+=2) {
          if (bl >= lengths[k]  &&  bl <= lengths[k+1]) {
            match(i,j) = True;
            ++nr;
          }
        }
      }
    }
    vector<Int> ant1, ant2;
    for (Int j=0; j<blength.shape()[1]; ++j) {
      for (Int i=0; i<blength.shape()[0]; ++i) {
        if (match(i,j)) {
          ant1.push_back (i);
          ant2.push_back (j);
        }
      }
    }
    TableExprNode condition(False);
    if (ant1.size() > 0) {
      Array<Int> arrAnt1(IPosition(1,ant1.size()), &(ant1[0]), SHARE);
      Array<Int> arrAnt2(IPosition(1,ant1.size()), &(ant2[0]), SHARE);
      condition = TableExprNode(any((ms()->col(colName1) == arrAnt1  &&
                                     ms()->col(colName2) == arrAnt2)));
    }
    setTEN (condition, AutoCorrAlso, negate);
    cout << "created node" << endl;
    return node();
  }

  Matrix<double> MSAntennaParse::getBaselineLengths()
  {
    MSAntenna msant(ms()->antenna());
    ROMSAntennaColumns antCols(msant);
    // First get the antenna positions.
    vector<Vector<double> > antVec;
    antVec.reserve (msant.nrow());
    for (uInt i=0; i<msant.nrow(); ++i) {
      // Convert to ITRF and keep as x,y,z in m.
      antVec.push_back
        (MPosition::Convert(antCols.positionMeas()(i),
                            MPosition::ITRF)().getValue().getValue());
    }
    // Fill in the length of each baseline.
    Matrix<double> blength(antVec.size(), antVec.size());
    for (uInt j=0; j<antVec.size(); ++j) {
      for (uInt i=0; i<antVec.size(); ++i) {
        Array<double> diff(antVec[i] - antVec[j]);
        blength(i,j) = sqrt(sum(diff*diff));
      }
    }
    return blength;
  }

  double MSAntennaParse::getUnitFactor (const char* unit)
  {
    // Check if conversion is possible.
    Unit u(unit);
    Quantity q(1., "m");
    if (! q.isConform (u)) {
      throw MSSelectionAntennaParseError (String("Unit ") + unit +
                                          " must be a distance unit (like m)");
    }
    // Get conversion factor.
    return q.getValue (unit);
  }

  const TableExprNode* MSAntennaParse::node()
  {
    return &node_p;
  }

  Bool MSAntennaParse::addBaseline(const Matrix<Int>& baselist, const Int ant1, const Int ant2, 
 				   BaselineListType autoCorr)
  {
    Bool doAutoCorr;
    doAutoCorr = (autoCorr==AutoCorrAlso) || (autoCorr==AutoCorrOnly);
    if ((ant1 == ant2) && (!doAutoCorr)) return False;
    if ((autoCorr==AutoCorrOnly) && (ant1!=ant2)) return False;

    Int n=baselist.shape()(0);
    for (Int i=0;i<n;i++) {
      if (((baselist(i,0)==ant1) && (baselist(i,1)==ant2)) ||
          ((baselist(i,1)==ant1) && (baselist(i,0)==ant2))) {
        return False;
      }
    }
    return True;
  }

  //
  // Method to make a list of unique baselines, give a list of
  // antenna1 and antenna2.  The baselines list is appended to the
  // existing list.  The required sizing could be done better.
  //
  void MSAntennaParse::makeBaselineList(const Vector<Int>& a1, const Vector<Int>&a2, 
					Matrix<Int>& baselist, 
					BaselineListType autoCorr,
					Bool /*negate*/)
  {
    Int n1,n2,nb0;
    n1=a1.nelements();  n2=a2.nelements();
    nb0=baselist.shape()(0);
    IPosition newSize(2,nb0,2);

    for (Int i1=0;i1<n1;i1++) {
      for (int i2=0;i2<n2;i2++) {
        Int ant1, ant2;
        ant1=a1[i1]; ant2=a2[i2];
        if (addBaseline(baselist,ant1,ant2,autoCorr)) {
          nb0++;
          newSize[0]=nb0;
          baselist.resize(newSize,True);
          baselist(nb0-1,0)=ant1;
          baselist(nb0-1,1)=ant2;
        }
      }
    }
  }
  
} //# NAMESPACE CASA - END
