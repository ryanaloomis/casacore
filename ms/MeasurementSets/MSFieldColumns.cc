//# MSFieldColumns.cc:  provides easy access to MeasurementSet columns
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

#include <casacore/ms/MeasurementSets/MSFieldColumns.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/ms/MeasurementSets/MSField.h>
#include <casacore/measures/Measures/MeasRef.h>
#include <casacore/casa/Quanta/MVDirection.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/tables/Tables/ColDescSet.h>
#include <casacore/tables/Tables/TableDesc.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/iomanip.h>

namespace casacore { //# NAMESPACE CASACORE - BEGIN


MSFieldColumns::MSFieldColumns()
{
}

MSFieldColumns::MSFieldColumns(const MSField& msField)
{
  attach(msField);
}

MSFieldColumns::~MSFieldColumns()
{
  // EPHEM
  for (size_t i=0; i<measCometsV_p.size(); i++) {
    delete measCometsV_p(i);
  }
}

void MSFieldColumns::attach(const MSField& msField)
{
  name_p.attach(msField, MSField::columnName(MSField::NAME));
  code_p.attach(msField, MSField::columnName(MSField::CODE));
  time_p.attach(msField, MSField::columnName(MSField::TIME));
  numPoly_p.attach(msField, MSField::columnName(MSField::NUM_POLY));
  delayDir_p.attach(msField, MSField::columnName(MSField::DELAY_DIR));
  phaseDir_p.attach(msField, MSField::columnName(MSField::PHASE_DIR));
  referenceDir_p.attach(msField,
			MSField::columnName(MSField::REFERENCE_DIR));
  sourceId_p.attach(msField, MSField::columnName(MSField::SOURCE_ID));
  flagRow_p.attach(msField, MSField::columnName(MSField::FLAG_ROW));
  timeMeas_p.attach(msField, MSField::columnName(MSField::TIME));
  delayDirMeas_p.attach(msField,MSField::columnName(MSField::DELAY_DIR));
  phaseDirMeas_p.attach(msField,MSField::columnName(MSField::PHASE_DIR));
  referenceDirMeas_p.attach(msField,
			    MSField::columnName(MSField::REFERENCE_DIR));
  timeQuant_p.attach(msField, MSField::columnName(MSField::TIME));
  attachOptionalCols(msField);
}

void MSFieldColumns::attachOptionalCols(const MSField& msField)
{
  const ColumnDescSet& cds = msField.tableDesc().columnDescSet();
  const String& ephemerisId = MSField::columnName(MSField::EPHEMERIS_ID);
  if (cds.isDefined(ephemerisId)){
    ephemerisId_p.attach(msField, ephemerisId);
    measCometsPath_p = Path(msField.tableName()).absoluteName();
    updateMeasComets();
  }
}

MDirection MSFieldColumns::
interpolateDirMeas(const Array<MDirection>& arrDir, Int numPoly, 
		   Double interTime, Double timeOrigin)
{
  Vector<MDirection> vecDir(arrDir);
  if ((numPoly == 0) || interTime<1 || nearAbs(interTime, timeOrigin)) {
    return vecDir(0);
  } else {
    Vector<Double> dir(vecDir(0).getAngle().getValue()), tmp; 
    Double dt = interTime - timeOrigin;
    Double fac = 1;
    for (Int i=1; i<(numPoly+1); i++) {
      fac *= dt;
      tmp = vecDir(i).getAngle().getValue();
      tmp *= fac;
      dir += tmp;
    }
    return MDirection(MVDirection(dir),vecDir(0).getRef());
  }
}

void MSFieldColumns::setEpochRef(MEpoch::Types ref, Bool tableMustBeEmpty) {
  timeMeas_p.setDescRefCode(ref, tableMustBeEmpty);
}

void MSFieldColumns::setDirectionRef(MDirection::Types ref) {
  delayDirMeas_p.setDescRefCode(ref);
  phaseDirMeas_p.setDescRefCode(ref); 
  referenceDirMeas_p.setDescRefCode(ref);
}

MDirection MSFieldColumns::delayDirMeas(rownr_t row, Double interTime) const
{
  Int npoly = numPoly()(row);
  if(npoly>0){
    return MSFieldColumns::interpolateDirMeas(delayDirMeasCol()(row), 
					      npoly,
					      interTime, time()(row));
  }
  else{
    Vector<MDirection> vecDir(delayDirMeasCol()(row));
    return extractDirMeas(vecDir(0),
			  measCometIndex(row),
			  interTime, timeMeas()(row));
  }
}

MDirection MSFieldColumns::phaseDirMeas(rownr_t row, Double interTime) const
{
  Int npoly = numPoly()(row);
  if(npoly>0){
    return MSFieldColumns::interpolateDirMeas(phaseDirMeasCol()(row), 
					      npoly,
					      interTime, time()(row));
  }
  else{
    Vector<MDirection> vecDir(phaseDirMeasCol()(row));
    return extractDirMeas(vecDir(0),
			  measCometIndex(row),
			  interTime, timeMeas()(row));
  }
}

MDirection MSFieldColumns::referenceDirMeas(rownr_t row, Double interTime) const
{
  Int npoly = numPoly()(row);
  if(npoly>0){
    return MSFieldColumns::interpolateDirMeas(referenceDirMeasCol()(row), 
					      npoly,
					      interTime, time()(row));
  }
  else{
    Vector<MDirection> vecDir(referenceDirMeasCol()(row));
    return extractDirMeas(vecDir(0),
			  measCometIndex(row),
			  interTime, timeMeas()(row));
  }
}

MDirection MSFieldColumns::ephemerisDirMeas(rownr_t row, Double interTime) const
{
  if(measCometIndex(row)>=0){
    const MDirection zeroDir = MDirection(Quantity(0, "deg"), Quantity(0, "deg"));
    return extractDirMeas(zeroDir,
			  measCometIndex(row),
			  interTime, timeMeas()(row));
  }
  else{
    return referenceDirMeas(row, interTime);
  }
}


MRadialVelocity MSFieldColumns::radVelMeas(rownr_t row, Double interTime) const
{
  MRadialVelocity rval;

  if( measCometsV_p.size()>0 ){

    Int index = measCometIndex(row);
    if(index>=0){
      Double originMJD, interMJD;
      getMJDs(originMJD, interMJD, interTime, timeMeas()(row));

      MVRadialVelocity mvradvel;
    
      if(!measCometsV_p(index)->getRadVel(mvradvel, interMJD)){
	stringstream ss;
	ss << "MSFieldColumns::radVelMeas(...) - No valid ephemeris entry for MJD " 
	   << setprecision(11) << interMJD << " for field " << row;
	throw(AipsError(ss.str()));
      }

      MRadialVelocity::Types mType = MRadialVelocity::TOPO;
      
      switch(measCometsV_p(index)->getType()){
      case MDirection::TOPO:
	break;
      case MDirection::APP:
      default:
	mType = MRadialVelocity::GEO;
	break;
      }
	
      return MRadialVelocity(mvradvel, mType);

    }

  }
  return rval;  
}

Quantity MSFieldColumns::rho(rownr_t row, Double interTime) const
{

  Quantity rval(0.,"m");

  if( measCometsV_p.size()>0 ){

    Int index = measCometIndex(row);
    if(index>=0){
      Double originMJD, interMJD;
      getMJDs(originMJD, interMJD, interTime, timeMeas()(row));
    
      MVPosition mvpos;
      if(!measCometsV_p(index)->get(mvpos, interMJD)){
	stringstream ss;
	ss << "MSFieldColumns::rho(...) - No valid ephemeris entry for MJD " 
	   << setprecision(11) << interMJD << " for field " << row;
	throw(AipsError(ss.str()));
      }
      rval = Quantity(mvpos.get()(0), "m");
    }

  }
  return rval;    

}

Bool MSFieldColumns::needInterTime(rownr_t row) const
{
  if( ( measCometsV_p.size()>0 && ephemerisId()(row)>=0 )
      || (numPoly()(row)>0) 
      ){
    return True;
  }
  return False;
}

Int MSFieldColumns::measCometIndex(rownr_t row) const
{
  Int rval = -1;
  if( measCometsV_p.size()>0 ){
    Int ephId = ephemerisId()(row);
    if(ephId>=0 && ephIdToMeasComet_p.find(ephId) != ephIdToMeasComet_p.end()){
      rval = ephIdToMeasComet_p.at(ephId);
    }
  }
  return rval;
}

String MSFieldColumns::ephemPath(rownr_t row) const
{
  String rval = "";
  Int index = measCometIndex(row);
  if( index>=0 ){
    rval = measCometsV_p(index)->getTablePath();
  }
  return rval;
}

Bool MSFieldColumns::
matchReferenceDir(rownr_t row, const MVDirection& dirVal, const Double& sepInRad, 
		  MVDirection& mvdir, Double time) const 
{
  try{
    mvdir = referenceDirMeas(row, time).getAngle();
  }
  catch(std::exception& x){
    return False;
  }
  if (dirVal.separation(mvdir) < sepInRad) {
    return True;
  } else {
    return False;
  }
}

Bool MSFieldColumns::
matchDelayDir(rownr_t row, const MVDirection& dirVal, const Double& sepInRad, 
	      MVDirection& mvdir, Double time) const 
{
  try{
    mvdir = delayDirMeas(row, time).getAngle();
  }
  catch(std::exception& x){
    return False;
  }
  if (dirVal.separation(mvdir) < sepInRad) {
    return True;
  } else {
    return False;
  }
}

Bool MSFieldColumns::
matchPhaseDir(rownr_t row, const MVDirection& dirVal, const Double& sepInRad, 
	      MVDirection& mvdir, Double time) const 
{
  try{
    mvdir = phaseDirMeas(row, time).getAngle();
  }
  catch(std::exception& x){
    return False;
  }
  if (dirVal.separation(mvdir) < sepInRad) {
    return True;
  } else {
    return False;
  }
}

Int64 MSFieldColumns::matchDirection(const MDirection& referenceDirection,
                                     const MDirection& delayDirection,
                                     const MDirection& phaseDirection,
                                     const Quantum<Double>& maxSeparation,
                                     Int64 tryRow, Double time) {
  rownr_t r = nrow();
  if (r == 0) return -1;
  const MVDirection& referenceDirVal = referenceDirection.getValue();
  const MVDirection& delayDirVal = delayDirection.getValue();
  const MVDirection& phaseDirVal = phaseDirection.getValue();
  // Convert the maximum separation to radians
  const Unit rad("rad");
  DebugAssert(maxSeparation.check(UnitVal::ANGLE), AipsError);
  const Double tolInRad = maxSeparation.getValue(rad);

  // Main matching loop
  MVDirection mvdir;
  if (tryRow >= 0) {
    const rownr_t tr = tryRow;
    if (tr >= r) {
      throw(AipsError("MSFieldColumns::matchDirection(...) - "
		      "row " + String::toString(tr) +
                      " you suggest is too big"));
    }
    if (!flagRow()(tr) &&
	numPoly()(tr) == 0){
      // Get the reference frame
      const MDirection::Types refType = 
	MDirection::castType(referenceDirMeas(tr).getRef().getType());
      // for a solar system object only the frame has to match
      if((refType>=MDirection::MERCURY && refType<MDirection::N_Planets) ||
	 (matchReferenceDir(tr, referenceDirVal, tolInRad, mvdir, time) &&
	  matchDelayDir(tr, delayDirVal, tolInRad, mvdir, time) &&
	  matchPhaseDir(tr, phaseDirVal, tolInRad, mvdir, time))
	 ) {
	if ((MDirection::castType(referenceDirection.getRef().getType())==refType) &&
	    (MDirection::castType(delayDirection.getRef().getType()) == refType) &&
	    (MDirection::castType(phaseDirection.getRef().getType()) == refType)) {
	  return tr;
	}
      }
    }
    if (tr == r-1) r--;
  }
  while (r > 0) {
    r--;
    if (!flagRow()(r) &&
	numPoly()(r) == 0){
      // Get the reference frame
      const MDirection::Types refType = 
	MDirection::castType(referenceDirMeas(r).getRef().getType());
      // for a solar system object only the frame has to match
      if((refType>=MDirection::MERCURY && refType<MDirection::N_Planets) ||
	 (matchReferenceDir(r, referenceDirVal, tolInRad, mvdir, time) &&
	  matchDelayDir(r, delayDirVal, tolInRad, mvdir, time) &&
	  matchPhaseDir(r, phaseDirVal, tolInRad, mvdir, time))
	  ) {
	if ((MDirection::castType(referenceDirection.getRef().getType())==refType) &&
	    (MDirection::castType(delayDirection.getRef().getType()) == refType) &&
	    (MDirection::castType(phaseDirection.getRef().getType()) == refType)) {
	  return r;
	}
      }
    }
  }
  return -1;
}

void MSFieldColumns::updateMeasComets()
{
  // delete old MeasComet objects
  for(size_t i=0; i<measCometsV_p.size(); i++){
    delete measCometsV_p(i);
  }
  measCometsV_p.resize(0);
  ephIdToMeasComet_p.clear();

  if(measCometsPath_p.length()==0){
    return;
  }

  // (re)create all necessary MeasComet objects
  Vector<Int> ephId = ephemerisId_p.getColumn();
  for(size_t i=0; i<ephId.size(); i++){
    Int theEphId = ephId(i);
    //cout << "updateMeasComet: processing row " << i << ", found eph id " << theEphId << endl;
    if(theEphId>=0 
       && ephIdToMeasComet_p.find(theEphId) == ephIdToMeasComet_p.end()){
      // the id is not yet in use, need to create a new MeasComet object
      
      // find the table belonging to this id
      Directory fieldDir(measCometsPath_p);
      stringstream ss;
      ss << theEphId;
      Regex ephemTableRegex (Regex::fromPattern("EPHEM"+ss.str()+"_*\\.tab"));
      Vector<String> candidates = fieldDir.find(ephemTableRegex, True, False); // followSymLinks=True, recursive=False
      if(candidates.size()==0){
	throw(AipsError("Ephemeris table "+ephemTableRegex.regexp()+" not found in "+measCometsPath_p));
      }
      String ephemTablePath = measCometsPath_p+"/"+candidates(0);
      if(!Table::isReadable(ephemTablePath)){
	throw(AipsError("Ephemeris table "+ephemTablePath+" is not readable."));
      }
      // create the new MeasComet object and store pointer to it in measCometsV_p
      MeasComet* mC = new MeasComet(ephemTablePath);
      size_t nMeasCom = measCometsV_p.size();
      measCometsV_p.resize(nMeasCom+1, True);
      measCometsV_p(nMeasCom) = mC;
      // remember the connection ephId to the measCometsV_p index
      ephIdToMeasComet_p.insert(std::make_pair(theEphId, nMeasCom)); 
      //cout << "Found and successfully opened ephemeris table " << ephemTablePath << endl;
    }
  }
} 


MDirection MSFieldColumns::extractDirMeas(const MDirection& offsetDir, 
                                          Int index, Double& interTime, 
                                          MEpoch originEpoch) const
{
  // this method is only called if numpoly==0

  if(index<0){ // no ephemeris available
    return offsetDir;
  }
  else{

    Double originMJD, interMJD;
    getMJDs(originMJD, interMJD, interTime, originEpoch);
    
    MVPosition xmvpos;
    if(!measCometsV_p(index)->get(xmvpos, interMJD)){
      stringstream ss;
      ss << "MSFieldColumns::extractDirMeas(...) - No valid ephemeris entry for MJD " 
	 << setprecision(11) << interMJD << " in ephemeris " << measCometsV_p(index)->getTablePath();
      throw(AipsError(ss.str()));
    }

    MVDirection mvxdir(xmvpos.getAngle());
    MVDirection mvodir(offsetDir.getAngle());
    
    mvxdir.shift(offsetDir.getAngle(), True); // shift in true angle, i.e. correcting for DEC
    
    return MDirection(mvxdir, measCometsV_p(index)->getType());
  }
}

void MSFieldColumns::getMJDs(Double& originMJD, Double& interMJD, 
                             const Double interTime, const MEpoch originEpoch) const
{
  // assume the same time reference frame of originEpoch and interTime
  MEpoch::Types assumedType = MEpoch::castType(originEpoch.getRef().getType());
  Unit days("d");

  if(assumedType==MEpoch::UTC){
    originMJD = originEpoch.get(days).getValue();
    interMJD = interTime/86400.;
  }
  else{
    originMJD= MEpoch::Convert(originEpoch,  MEpoch::UTC)().get(days).getValue();
    MEpoch interEpoch(Quantity(interTime, "s"), assumedType);
    interMJD = MEpoch::Convert(interEpoch, MEpoch::UTC)().get(days).getValue();
  }
  if(interTime==0.){
    interMJD = originMJD;
  }
}


} //# NAMESPACE CASACORE - END
