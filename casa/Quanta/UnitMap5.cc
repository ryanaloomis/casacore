//# UnitMap5.cc: Unit map custom units initialisation part 1
//# Copyright (C) 2001,2002
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

//# Includes

#include <casacore/casa/Quanta/UnitMap.h>

namespace casacore { //# NAMESPACE CASACORE - BEGIN

// Initialise the maps
void UnitMap::initUMCust1 (UMaps& maps) {
  map<String, UnitName>& mapCust = maps.mapCust;
  mapCust.insert(map<String, UnitName>::value_type
			   ("sq_deg", 	UnitName("sq_deg",
						 UnitVal(C::square_degree, "sr", &maps),
						 "square degree")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("sq_arcmin", UnitName("sq_arcmin",
						  UnitVal(C::square_arcmin, "sr", &maps),
						  "square arcmin")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("sq_arcsec", UnitName("sq_arcsec",
						  UnitVal(C::square_arcsec, "sr", &maps),
						  "square arcsec")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("deg_2",  	UnitName("deg_2",
						 UnitVal(C::square_degree, "sr", &maps),
						 "square degree")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("arcmin_2",  UnitName("arcmin_2",
						  UnitVal(C::square_arcmin,"sr", &maps),
						  "square arcmin")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("arcsec_2",  UnitName("arcsec_2",
						  UnitVal(C::square_arcsec, "sr", &maps),
						  "square arcsec")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("'",   	UnitName("'",
						 UnitVal(C::arcmin, "rad", &maps),
						 "arcmin")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("''",   	UnitName("''",
						 UnitVal(C::arcsec, "rad", &maps),
						 "arcsec")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("\"",   	UnitName("\"",
						 UnitVal(C::arcsec, "rad", &maps),
						 "arcsec")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("'_2",  	UnitName("'_2",
						 UnitVal(C::square_arcmin, "sr", &maps),
						 "square arcmin")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("''_2",  	UnitName("''_2",
						 UnitVal(C::square_arcsec, "sr", &maps),
						 "square arcsec")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("\"_2",  	UnitName("\"_2",
						 UnitVal(C::square_arcsec, "sr", &maps),
						 "square arcsec")));
  mapCust.insert(map<String, UnitName>::value_type
			   (":",   	UnitName(":",
						 UnitVal(1., "h", &maps),
						 "hour")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("::",   	UnitName("::",
						 UnitVal(1., "min", &maps),
						 "minute")));
  mapCust.insert(map<String, UnitName>::value_type
			   (":::",   	UnitName(":::",
						 UnitVal(1., "s", &maps),
						 "second")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("FU",   	UnitName("FU",
						 UnitVal(1., "Jy", &maps),
						 "flux unit")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("fu",   	UnitName("fu",
						 UnitVal(1., "FU", &maps),
						 "flux unit")));
  mapCust.insert(map<String, UnitName>::value_type
			   ("WU",   	UnitName("WU",
						 UnitVal(5., "mJy", &maps),
						 "WSRT flux unit")));
}

} //# NAMESPACE CASACORE - END

