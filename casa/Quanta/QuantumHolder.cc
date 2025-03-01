//# QuantumHolder.cc: A holder for Quantum to enable record conversions
//# Copyright (C) 1998,1999,2000,2001
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
#include <casacore/casa/Quanta/QuantumHolder.h>
#include <casacore/casa/Exceptions.h>
#include <casacore/casa/Quanta/UnitVal.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Containers/RecordInterface.h>
#include <casacore/casa/Containers/RecordFieldId.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/IO/ArrayIO.h>

namespace casacore { //# NAMESPACE CASACORE - BEGIN

//# Constructors
QuantumHolder::QuantumHolder() 
  : hold_p() {}

QuantumHolder::QuantumHolder(const QBase &in) 
  : hold_p(in.clone()) {}

QuantumHolder::QuantumHolder(const QuantumHolder &other) 
  : RecordTransformable(),
    hold_p()
{
  if (other.hold_p) {
    hold_p.reset (other.hold_p->clone());
  }
}

//# Destructor
QuantumHolder::~QuantumHolder() {}

//# Operators
QuantumHolder &QuantumHolder::operator=(const QuantumHolder &other) {
  if (this != &other) {
    if (other.hold_p) {
      hold_p.reset (other.hold_p->clone());
    } else {
      hold_p.reset();
    }
  }
  return *this;
}

//# Member Functions
Bool QuantumHolder::isEmpty() const {
  return (!hold_p);
}

Bool QuantumHolder::isQuantum() const {
  return (static_cast<Bool>(hold_p));
}

Bool QuantumHolder::isScalar() const {
  return (hold_p && nelements() == 1);
}

Bool QuantumHolder::isVector() const {
  return (hold_p && ndim() == 1);
}

Bool QuantumHolder::isArray() const {
  return (hold_p && ndim() > 0);
}

Bool QuantumHolder::isReal() const {
  return (hold_p &&
		(isQuantumDouble() ||
		 isQuantumFloat() ||
		 isQuantumInt() ||
		 isQuantumArrayDouble() ||
		 isQuantumArrayFloat() ||
		 isQuantumArrayInt()));
}

Bool QuantumHolder::isComplex() const {
  return (hold_p &&
		(isQuantumComplex() ||
		 isQuantumDComplex() ||
		 isQuantumArrayComplex() ||
		 isQuantumArrayDComplex()));
}

Bool QuantumHolder::isQuantity() const {
  return (hold_p && isQuantumDouble());
}

Bool QuantumHolder::isQuantumDouble() const {
  return (hold_p &&
		hold_p->type() == Quantum<Double>::myType());
}

Bool QuantumHolder::isQuantumFloat() const {
  return (hold_p &&
		hold_p->type() == Quantum<Float>::myType());
}

Bool QuantumHolder::isQuantumInt() const {
  return (hold_p &&
		hold_p->type() == Quantum<Int>::myType());
}

Bool QuantumHolder::isQuantumComplex() const {
  return (hold_p &&
		hold_p->type() == Quantum<Complex>::myType());
}

Bool QuantumHolder::isQuantumDComplex() const {
  return (hold_p &&
		hold_p->type() == Quantum<DComplex>::myType());
}

Bool QuantumHolder::isQuantumArrayDouble() const {
  return (hold_p &&
		(hold_p->type() == Quantum<Array<Double>>::myType() ||
		 hold_p->type() == Quantum<Vector<Double>>::myType()));
}

Bool QuantumHolder::isQuantumArrayFloat() const {
  return (hold_p &&
  		(hold_p->type() == Quantum<Array<Float>>::myType() ||
		 hold_p->type() == Quantum<Vector<Float>>::myType()));
}

Bool QuantumHolder::isQuantumArrayInt() const {
  return (hold_p &&
  		(hold_p->type() == Quantum<Array<Int>>::myType() ||
		 hold_p->type() == Quantum<Vector<Int>>::myType()));
}

Bool QuantumHolder::isQuantumArrayComplex() const {
  return (hold_p &&
  		(hold_p->type() == Quantum<Array<Complex>>::myType() ||
		 hold_p->type() == Quantum<Vector<Complex>>::myType()));
}

Bool QuantumHolder::isQuantumArrayDComplex() const {
  return (hold_p &&
  		(hold_p->type() == Quantum<Array<DComplex>>::myType() ||
		 hold_p->type() == Quantum<Vector<DComplex>>::myType()));
}

Bool QuantumHolder::isQuantumVectorDouble() const {
  return (isQuantumArrayDouble() && ndim() == 1);
}

Bool QuantumHolder::isQuantumVectorFloat() const {
  return (isQuantumArrayFloat() && ndim() == 1);
}

Bool QuantumHolder::isQuantumVectorInt() const {
  return (isQuantumArrayInt() && ndim() == 1);
}

Bool QuantumHolder::isQuantumVectorComplex() const {
  return (isQuantumArrayComplex() && ndim() == 1);
}

Bool QuantumHolder::isQuantumVectorDComplex() const {
  return (isQuantumArrayDComplex() && ndim() == 1);
}

Int QuantumHolder::nelements() const {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for nelements"));
  } else if (isQuantumArrayDouble()) {
    return (static_cast<Quantum<Array<Double>>*>(hold_p.get()))->getValue().nelements();
  } else if (isQuantumArrayFloat()) {
    return (static_cast<Quantum<Array<Float>>*>(hold_p.get()))->getValue().nelements();
  } else if (isQuantumArrayInt()) {
    return (static_cast<Quantum<Array<Int>>*>(hold_p.get()))->getValue().nelements();
  } else if (isQuantumArrayComplex()) {
    return (static_cast<Quantum<Array<Complex>>*>(hold_p.get()))->getValue().nelements();
  } else if (isQuantumArrayDComplex()) {
    return (static_cast<Quantum<Array<DComplex>>*>(hold_p.get()))->getValue().nelements();
  }
  return 1;
}

Int QuantumHolder::ndim() const {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for ndim"));
  } else if (isQuantumArrayDouble()) {
    return (static_cast<Quantum<Array<Double>>*>(hold_p.get()))->getValue().ndim();
  } else if (isQuantumArrayFloat()) {
    return (static_cast<Quantum<Array<Float>>*>(hold_p.get()))->getValue().ndim();
  } else if (isQuantumArrayInt()) {
    return (static_cast<Quantum<Array<Int>>*>(hold_p.get()))->getValue().ndim();
  } else if (isQuantumArrayComplex()) {
    return (static_cast<Quantum<Array<Complex>>*>(hold_p.get()))->getValue().ndim();
  } else if (isQuantumArrayDComplex()) {
    return (static_cast<Quantum<Array<DComplex>>*>(hold_p.get()))->getValue().ndim();
  }
  return 0;
}

const QBase &QuantumHolder::asQuantum() const {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantum"));
  }
  return *hold_p;
}

const Quantum<Double> &QuantumHolder::asQuantity() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumDouble"));
  }
  if (!isReal() || !isScalar()) {
    throw(AipsError("Wrong QuantumHolder to convert asQuantumDouble"));
  }
  if (!isQuantity()) toReal(Quantum<Double>::myType());
  return static_cast<const Quantum<Double>&>(*hold_p);
}

const Quantum<Double> &QuantumHolder::asQuantumDouble() {
  return asQuantity();
}

const Quantum<Float> &QuantumHolder::asQuantumFloat() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumFloat"));
  }
  if (!isReal() || !isScalar()) {
    throw(AipsError("Wrong QuantumHolder to convert asQuantumFloat"));
  }
  if (!isQuantumFloat()) toReal(Quantum<Float>::myType());
  return static_cast<const Quantum<Float>&>(*hold_p);
}

const Quantum<Int> &QuantumHolder::asQuantumInt() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumInt"));
  }
  if (!isReal() || !isScalar()) {
    throw(AipsError("Wrong QuantumHolder to convert asQuantumInt"));
  }
  if (!isQuantumInt()) toReal(Quantum<Int>::myType());
  return static_cast<const Quantum<Int>&>(*hold_p);
}

const Quantum<Complex> &QuantumHolder::asQuantumComplex() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumComplex"));
  }
  if (!isScalar()) {
    throw(AipsError("Wrong QuantumHolder to convert asQuantumComplex"));
  }
  if (!isQuantumComplex()) toComplex(Quantum<Complex>::myType());
  return static_cast<const Quantum<Complex>&>(*hold_p);
}

const Quantum<DComplex> &QuantumHolder::asQuantumDComplex() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumDComplex"));
  }
  if (!isScalar()) {
    throw(AipsError("Wrong QuantumHolder to convert asQuantumDComplex"));
  }
  if (!isQuantumDComplex()) toComplex(Quantum<DComplex>::myType());
  return static_cast<const Quantum<DComplex>&>(*hold_p);
}

const Quantum<Vector<Double>> &QuantumHolder::asQuantumVectorDouble() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumVectorDouble"));
  }
  if (isArray()) {
    if (!isQuantumArrayDouble()) {
      throw(AipsError("Cannot convert to QuantumVectorDouble"));
    }
    if (ndim() != 1) {
      (static_cast<Quantum<Array<Double>>*>(hold_p.get()))->getValue().
	reform(IPosition(1, nelements()));
    }
  } else {
    if (!isReal()) {
      throw(AipsError("Wrong QuantumHolder to convert asQuantumVectorDouble"));
    }
    if (!isQuantumDouble()) toReal(Quantum<Double>::myType());
    toVector();
  }
  return static_cast<const Quantum<Vector<Double>>&>(*hold_p);
}

const Quantum<Vector<Float>> &QuantumHolder::asQuantumVectorFloat() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumVectorFloat"));
  }
  if (isArray()) {
    if (!isQuantumArrayFloat()) {
      throw(AipsError("Cannot convert to QuantumVectorFloat"));
    }
    if (ndim() != 1) {
      (static_cast<Quantum<Array<Float>>*>(hold_p.get()))->getValue().
	reform(IPosition(1, nelements()));
    }
  } else {
    if (!isReal()) {
      throw(AipsError("Wrong QuantumHolder to convert asQuantumVectorFloat"));
    }
    if (!isQuantumFloat()) toReal(Quantum<Float>::myType());
    toVector();
  }
  return static_cast<const Quantum<Vector<Float>>&>(*hold_p);
}

const Quantum<Vector<Int>> &QuantumHolder::asQuantumVectorInt() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumVectorInt"));
  }
  if (isArray()) {
    if (!isQuantumArrayInt()) {
      throw(AipsError("Cannot convert to QuantumVectorInt"));
    }
    if (ndim() != 1) {
      (static_cast<Quantum<Array<Int>>*>(hold_p.get()))->getValue().
	reform(IPosition(1, nelements()));
    }
  } else {
    if (!isReal()) {
      throw(AipsError("Wrong QuantumHolder to convert asQuantumVectorInt"));
    }
    if (!isQuantumInt()) toReal(Quantum<Int>::myType());
    toVector();
  }
  return static_cast<const Quantum<Vector<Int>>&>(*hold_p);
}

const Quantum<Vector<Complex>> &QuantumHolder::asQuantumVectorComplex() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumVectorComplex"));
  }
  if (isArray()) {
    if (!isQuantumArrayComplex()) {
      throw(AipsError("Cannot convert to QuantumVectorComplex"));
    }
    if (ndim() != 1) {
      (static_cast<Quantum<Array<Complex>>*>(hold_p.get()))->getValue().
	reform(IPosition(1, nelements()));
    }
  } else {
    if (!isQuantumComplex()) toComplex(Quantum<Complex>::myType());
    toVector();
  }
  return static_cast<const Quantum<Vector<Complex>>&>(*hold_p);
}

const Quantum<Vector<DComplex>> &QuantumHolder::asQuantumVectorDComplex() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumVectorDComplex"));
  }
  if (isArray()) {
    if (!isQuantumArrayDComplex()) {
      throw(AipsError("Cannot convert to QuantumVectorDComplex"));
    }
    if (ndim() != 1) {
      (static_cast<Quantum<Array<DComplex>>*>(hold_p.get()))->getValue().
	reform(IPosition(1, nelements()));
    }
  } else {
    if (!isQuantumDComplex()) toComplex(Quantum<DComplex>::myType());
    toVector();
  }
  return static_cast<const Quantum<Vector<DComplex>>&>(*hold_p);
}

const Quantum<Array<Double>> &QuantumHolder::asQuantumArrayDouble() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumArrayDouble"));
  }
  if (isArray()) {
    if (!isQuantumArrayDouble()) {
      throw(AipsError("Cannot convert to QuantumArrayDouble"));
    }
  } else {
    if (!isReal()) {
      throw(AipsError("Wrong QuantumHolder to convert asQuantumArrayDouble"));
    }
    if (!isQuantumDouble()) toReal(Quantum<Double>::myType());
    toArray();
  }
  return static_cast<const Quantum<Array<Double>>&>(*hold_p);
}

const Quantum<Array<Float>> &QuantumHolder::asQuantumArrayFloat() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumArrayFloat"));
  }
  if (isArray()) {
    if (!isQuantumArrayFloat()) {
      throw(AipsError("Cannot convert to QuantumArrayFloat"));
    }
  } else {
    if (!isReal()) {
      throw(AipsError("Wrong QuantumHolder to convert asQuantumArrayFloat"));
    }
    if (!isQuantumFloat()) toReal(Quantum<Float>::myType());
    toArray();
  }
  return static_cast<const Quantum<Array<Float>>&>(*hold_p);
}

const Quantum<Array<Int>> &QuantumHolder::asQuantumArrayInt() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumArrayInt"));
  }
  if (isArray()) {
    if (!isQuantumArrayInt()) {
      throw(AipsError("Cannot convert to QuantumArrayInt"));
    }
  } else {
    if (!isReal()) {
      throw(AipsError("Wrong QuantumHolder to convert asQuantumArrayInt"));
    }
    if (!isQuantumInt()) toReal(Quantum<Int>::myType());
    toArray();
  }
  return static_cast<const Quantum<Array<Int>>&>(*hold_p);
}

const Quantum<Array<Complex>> &QuantumHolder::asQuantumArrayComplex() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumArrayComplex"));
  }
  if (isArray()) {
    if (!isQuantumArrayComplex()) {
      throw(AipsError("Cannot convert to QuantumArrayComplex"));
    }
  } else {
    if (!isQuantumComplex()) toComplex(Quantum<Complex>::myType());
    toArray();
  }
  return static_cast<const Quantum<Array<Complex>>&>(*hold_p);
}

const Quantum<Array<DComplex>> &QuantumHolder::asQuantumArrayDComplex() {
  if (!hold_p) {
    throw(AipsError("Empty QuantumHolder argument for asQuantumArrayDComplex"));
  }
  if (isArray()) {
    if (!isQuantumArrayDComplex()) {
      throw(AipsError("Cannot convert to QuantumArrayDComplex"));
    }
  } else {
    if (!isQuantumDComplex()) toComplex(Quantum<DComplex>::myType());
    toArray();
  }
  return static_cast<const Quantum<Array<DComplex>>&>(*hold_p);
}

Bool QuantumHolder::fromRecord(String &error,
			       const RecordInterface &in) {
  String un;
  if (in.isDefined(String("value")) && in.isDefined(String("unit")) &&
      in.type(in.idToNumber(RecordFieldId("unit"))) == TpString) {
    String un;
    in.get(RecordFieldId("unit"), un);
    switch (in.type(in.idToNumber(RecordFieldId("value")))) {
    case TpDouble:
    {
      Double vl;
      in.get(RecordFieldId("value"), vl);
      hold_p.reset (new Quantum<Double>(vl, un));
      return True;
    }
    case TpFloat:
    {
      Float vl;
      in.get(RecordFieldId("value"), vl);
      hold_p.reset (new Quantum<Float>(vl, un));
      return True;
    }
    case TpInt:
    {
      Int vl;
      in.get(RecordFieldId("value"), vl);
      hold_p.reset (new Quantum<Int>(vl, un));
      return True;
    }
    case TpComplex:
    {
      Complex vl;
      in.get(RecordFieldId("value"), vl);
      hold_p.reset (new Quantum<Complex>(vl, un));
      return True;
    }
    case TpDComplex:
    {
      DComplex vl;
      in.get(RecordFieldId("value"), vl);
      hold_p.reset (new Quantum<DComplex>(vl, un));
      return True;
    }
    case TpArrayDouble:
    {
      Array<Double> vl;
      in.get(RecordFieldId("value"), vl);
      hold_p.reset (new Quantum<Array<Double>>(vl, un));
      return True;
    }
    case TpArrayFloat:
    {
      Array<Float> vl;
      in.get(RecordFieldId("value"), vl);
      hold_p.reset (new Quantum<Array<Float>>(vl, un));
      return True;
    }
    case TpArrayInt:
    {
      Array<Int> vl;
      in.get(RecordFieldId("value"), vl);
      hold_p.reset (new Quantum<Array<Int>>(vl, un));
      return True;
    }
    case TpArrayComplex:
    {
      Array<Complex> vl;
      in.get(RecordFieldId("value"), vl);
      hold_p.reset (new Quantum<Array<Complex>>(vl, un));
      return True;
    }
    case TpArrayDComplex:
    {
      Array<DComplex> vl;
      in.get(RecordFieldId("value"), vl);
      hold_p.reset (new Quantum<Array<DComplex>>(vl, un));
      return True;
    }
    default:
      break;
    }
  }
  error += String("Illegal Quantum record in QuantumHolder::fromRecord\n");
  return False;
}

Bool QuantumHolder::fromString(String &error,
			       const String &in) {
  Quantum<Double> res;
  if (!Quantum<Double>::read(res, in)) {
    error += String("in QuantumHolder::fromString with input string \"") +
      in + String("\": Illegal input units or format\n");
    return False;
  }
  hold_p.reset (new Quantum<Double>(res));
  return True;
}

Bool QuantumHolder::toRecord(String &error, RecordInterface &out) const {
  if (hold_p) {
    if (out.isDefined("value")) out.removeField(RecordFieldId("value"));
    if (isQuantumDouble()) {
      out.define(RecordFieldId("value"),
		 ((static_cast<Quantum<Double>*>(hold_p.get()))->getValue()));
    } else if (isQuantumFloat()) {
      out.define(RecordFieldId("value"),
                 ((static_cast<Quantum<Float>*>(hold_p.get()))->getValue()));
    } else if (isQuantumInt()) {
      out.define(RecordFieldId("value"),
                 ((static_cast<Quantum<Int>*>(hold_p.get()))->getValue()));
    } else if (isQuantumComplex()) {
      out.define(RecordFieldId("value"),
                 ((static_cast<Quantum<Complex>*>(hold_p.get()))->getValue()));
    } else if (isQuantumDComplex()) {
      out.define(RecordFieldId("value"),
                 ((static_cast<Quantum<DComplex>*>(hold_p.get()))->getValue()));
    } else if (isQuantumVectorDouble()) {
      out.define(RecordFieldId("value"),
		 ((static_cast<Quantum<Vector<Double>>*>(hold_p.get()))->getValue()));
    } else if (isQuantumVectorFloat()) {
      out.define(RecordFieldId("value"),
		 ((static_cast<Quantum<Vector<Float>>*>(hold_p.get()))->getValue()));
    } else if (isQuantumVectorInt()) {
      out.define(RecordFieldId("value"),
		 ((static_cast<Quantum<Vector<Int>>*>(hold_p.get()))->getValue()));
    } else if (isQuantumVectorComplex()) {
      out.define(RecordFieldId("value"),
		 ((static_cast<Quantum<Vector<Complex>>*>(hold_p.get()))->getValue()));
    } else if (isQuantumVectorDComplex()) {
      out.define(RecordFieldId("value"),
		 ((static_cast<Quantum<Vector<DComplex>>*>(hold_p.get()))->getValue()));
    } else if (isQuantumArrayDouble()) {
      out.define(RecordFieldId("value"),
		 ((static_cast<Quantum<Array<Double>>*>(hold_p.get()))->getValue()));
    } else if (isQuantumArrayFloat()) {
      out.define(RecordFieldId("value"),
		 ((static_cast<Quantum<Array<Float>>*>(hold_p.get()))->getValue()));
    } else if (isQuantumArrayInt()) {
      out.define(RecordFieldId("value"),
		 ((static_cast<Quantum<Array<Int>>*>(hold_p.get()))->getValue()));
    } else if (isQuantumArrayComplex()) {
      out.define(RecordFieldId("value"),
		 ((static_cast<Quantum<Array<Complex>>*>(hold_p.get()))->getValue()));
    } else if (isQuantumArrayDComplex()) {
      out.define(RecordFieldId("value"),
		 ((static_cast<Quantum<Array<DComplex>>*>(hold_p.get()))->getValue()));
    }
    out.define(RecordFieldId("unit"), hold_p->getFullUnit().getName());
    return True;
  }
  error += String("No Quantum specified in QuantumHolder::toRecord\n");
  return False;
}

void QuantumHolder::toRecord(RecordInterface &out) const {
	String error;
	if (! toRecord(error, out)) {
		throw AipsError(error);
	}
}

Record QuantumHolder::toRecord() const {
	Record r;
	toRecord(r);
	return r;
}


void QuantumHolder::toReal(const uInt &tp) {
  Double d1=0;
  if (isArray()) {
    IPosition stx(ndim(), 0);
    if (isQuantumArrayDouble()) {
      d1 = static_cast<Quantum<Array<Double>>*>(hold_p.get())->getValue()(stx);
    } else if (isQuantumArrayFloat()) {
      d1 = static_cast<Quantum<Array<Float>>*>(hold_p.get())->getValue()(stx);
    } else if (isQuantumArrayInt()) {
      d1 = static_cast<Quantum<Array<Int>>*>(hold_p.get())->getValue()(stx);
    }
  } else {
    if (isQuantumDouble()) {
      d1 = static_cast<Quantum<Double>*>(hold_p.get())->getValue();
    } else if (isQuantumFloat()) {
      d1 = static_cast<Quantum<Float>*>(hold_p.get())->getValue();
    } else if (isQuantumInt()) {
      d1 = static_cast<Quantum<Int>*>(hold_p.get())->getValue();
    }
  }
  Unit x = hold_p->getFullUnit();
  if (tp == Quantum<Double>::myType()) {
    hold_p.reset (new Quantum<Double>(d1, x));
  } else if (tp == Quantum<Float>::myType()) {
    hold_p.reset (new Quantum<Float>(Float(d1), x));
  } else if (tp == Quantum<Int>::myType()) {
    hold_p.reset (new Quantum<Int>(Int(d1), x));
  }
}

void QuantumHolder::toComplex(const uInt &tp) {
  DComplex d1;
  if (isArray()) {
    IPosition stx(ndim(), 0);
    if (isQuantumArrayDouble()) {
      d1 = static_cast<Quantum<Array<Double>>*>(hold_p.get())->getValue()(stx);
    } else if (isQuantumArrayFloat()) {
      d1 = static_cast<Quantum<Array<Float>>*>(hold_p.get())->getValue()(stx);
    } else if (isQuantumArrayInt()) {
      d1 = static_cast<Quantum<Array<Int>>*>(hold_p.get())->getValue()(stx);
    } else if (isQuantumArrayComplex()) {
      d1 = static_cast<Quantum<Array<Complex>>*>(hold_p.get())->getValue()(stx);
    } else if (isQuantumArrayDComplex()) {
      d1 = static_cast<Quantum<Array<DComplex>>*>(hold_p.get())->getValue()(stx);
    }
  } else {
    if (isQuantumDouble()) {
      d1 = static_cast<Quantum<Double>*>(hold_p.get())->getValue();
    } else if (isQuantumFloat()) {
      d1 = static_cast<Quantum<Float>*>(hold_p.get())->getValue();
    } else if (isQuantumInt()) {
      d1 = static_cast<Quantum<Int>*>(hold_p.get())->getValue();
    } else if (isQuantumComplex()) {
      d1 = static_cast<Quantum<Complex>*>(hold_p.get())->getValue();
    } else if (isQuantumDComplex()) {
      d1 = static_cast<Quantum<DComplex>*>(hold_p.get())->getValue();
    }
  }
  Unit x = hold_p->getFullUnit();
  if (tp == Quantum<Complex>::myType()) {
    hold_p.reset (new Quantum<Complex>(Complex(d1), x));
  } else  if (tp == Quantum<DComplex>::myType()) {
    hold_p.reset (new Quantum<DComplex>(d1, x));
  }
}

void QuantumHolder::toVector() {
  Unit x = hold_p->getFullUnit();
  if (isQuantumDouble()) {
    Vector<Double> d1(1);
    d1(0) = static_cast<Quantum<Double>*>(hold_p.get())->getValue();
    hold_p.reset (new Quantum<Vector<Double>>(d1, x));
  } else if (isQuantumFloat()) {
    Vector<Float> d1(1);
    d1(0) = static_cast<Quantum<Float>*>(hold_p.get())->getValue();
    hold_p.reset (new Quantum<Vector<Float>>(d1, x));
  } else if (isQuantumInt()) {
    Vector<Int> d1(1);
    d1(0) = static_cast<Quantum<Int>*>(hold_p.get())->getValue();
    hold_p.reset (new Quantum<Vector<Int>>(d1, x));
  } else if (isQuantumComplex()) {
    Vector<Complex> d1(1);
    d1(0) = static_cast<Quantum<Complex>*>(hold_p.get())->getValue();
    hold_p.reset (new Quantum<Vector<Complex>>(d1, x));
  } else if (isQuantumDComplex()) {
    Vector<DComplex> d1(1);
    d1(0) = static_cast<Quantum<DComplex>*>(hold_p.get())->getValue();
    hold_p.reset (new Quantum<Vector<DComplex>>(d1, x));
  }
}

void QuantumHolder::toArray() {
  toVector();
}

const String &QuantumHolder::ident() const {
  static const String myid = "quant";
  return myid;
}

} //# NAMESPACE CASACORE - END

