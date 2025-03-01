//# ExprGroup.cc: Classes for TaQL's GROUPBY clause
//# Copyright (C) 2013
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
#include <casacore/tables/TaQL/ExprGroup.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include <casacore/tables/TaQL/ExprAggrNode.h>
#include <casacore/tables/TaQL/ExprAggrNodeArray.h>
#include <casacore/tables/TaQL/ExprUDFNode.h>
#include <casacore/tables/Tables/TableError.h>
#include <casacore/casa/Utilities/Sort.h>
#include <casacore/casa/Utilities/Assert.h>
#include <limits>


namespace casacore { //# NAMESPACE CASACORE - BEGIN

  bool TableExprGroupKey::operator== (const TableExprGroupKey& that) const
  {
    switch (itsDT) {
    case TableExprNodeRep::NTBool:
      return itsBool == that.itsBool;
    case TableExprNodeRep::NTInt:
      return itsInt64 == that.itsInt64;
    case TableExprNodeRep::NTDouble:
      return itsDouble == that.itsDouble;
    default:
      return itsString == that.itsString;
    }
  }

  bool TableExprGroupKey::operator< (const TableExprGroupKey& that) const
  {
    switch (itsDT) {
    case TableExprNodeRep::NTBool:
      return itsBool < that.itsBool;
    case TableExprNodeRep::NTInt:
      return itsInt64 < that.itsInt64;
    case TableExprNodeRep::NTDouble:
      return itsDouble < that.itsDouble;
    default:
      return itsString < that.itsString;
    }
  }


  TableExprGroupKeySet::TableExprGroupKeySet (const vector<TableExprNode>& nodes)
  {
    itsKeys.reserve (nodes.size());
    for (uInt i=0; i<nodes.size(); ++i) {
      addKey (nodes[i].getRep()->dataType());
    }
  }

  void TableExprGroupKeySet::fill (const vector<TableExprNode>& nodes,
                                   const TableExprId& id)
  {
    AlwaysAssert (nodes.size() == itsKeys.size(), AipsError);
    for (uInt i=0; i<itsKeys.size(); ++i) {
      switch (itsKeys[i].dataType()) {
      case TableExprNodeRep::NTBool:
        itsKeys[i].set (nodes[i].getBool(id));
        break;
      case TableExprNodeRep::NTInt:
        itsKeys[i].set (nodes[i].getInt(id));
        break;
      case TableExprNodeRep::NTDouble:
        itsKeys[i].set (nodes[i].getDouble(id));
        break;
      case TableExprNodeRep::NTString:
        itsKeys[i].set (nodes[i].getString(id));
        break;
      case TableExprNodeRep::NTDate:
        // Handle a date/time as a double.
        itsKeys[i].set (nodes[i].getDouble(id));
        break;
      default:
        throw TableInvExpr ("A GROUPBY key cannot have data type dcomplex");
      }
    }
  }

  bool TableExprGroupKeySet::operator== (const TableExprGroupKeySet& that) const
  {
    AlwaysAssert (itsKeys.size() == that.itsKeys.size(), AipsError);
    for (size_t i=0; i<itsKeys.size(); ++i) {
      if (!(itsKeys[i] == that.itsKeys[i])) return false;
    }
    return true;
  }

  bool TableExprGroupKeySet::operator< (const TableExprGroupKeySet& that) const
  {
    AlwaysAssert (itsKeys.size() == that.itsKeys.size(), AipsError);
    for (size_t i=0; i<itsKeys.size(); ++i) {
      if (itsKeys[i] < that.itsKeys[i]) return true;
      if (that.itsKeys[i] < itsKeys[i]) return false;
    }
    return false;
  }


  TableExprGroupResult::TableExprGroupResult
  (const vector<CountedPtr<TableExprGroupFuncSet> >& funcSets)
  {
    itsFuncSets = funcSets;
  }
  TableExprGroupResult::TableExprGroupResult
  (const vector<CountedPtr<TableExprGroupFuncSet> >& funcSets,
   const vector<CountedPtr<vector<TableExprId> > >& ids)
  {
    AlwaysAssert (ids.size() == funcSets.size()  ||  ids.empty(), AipsError);
    itsFuncSets = funcSets;
    itsIds      = ids;
  }


  TableExprGroupFuncBase::TableExprGroupFuncBase (TableExprNodeRep* node)
    : itsNode    (node),
      itsOperand (0),
      itsSeqnr   (0)
  {
    if (node) {
      TableExprAggrNode* snode = dynamic_cast<TableExprAggrNode*>(node);
      if (snode) {
        itsOperand = snode->operand().get();
      } else {
        TableExprAggrNodeArray* anode = dynamic_cast<TableExprAggrNodeArray*>(node);
        if (anode) {
          itsOperand = anode->operand().get();
        } else {
          TableExprUDFNode* unode = dynamic_cast<TableExprUDFNode*>(node);
          AlwaysAssert (unode  &&  unode->isAggregate(), AipsError);
        }
      }
    }
  }
  TableExprGroupFuncBase::~TableExprGroupFuncBase()
  {}
  Bool TableExprGroupFuncBase::isLazy() const
    { return False; }
  void TableExprGroupFuncBase::finish()
  {}
  CountedPtr<vector<TableExprId> > TableExprGroupFuncBase::getIds() const
  { throw TableInvExpr ("TableExprGroupFuncBase::getIds not implemented"); }
  Bool TableExprGroupFuncBase::getBool (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getBool not implemented"); }
  Int64 TableExprGroupFuncBase::getInt (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getInt not implemented"); }
  Double TableExprGroupFuncBase::getDouble (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getDouble not implemented"); }
  DComplex TableExprGroupFuncBase::getDComplex (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getDComplex not implemented"); }
  MVTime TableExprGroupFuncBase::getDate (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getDate not implemented"); }
  String TableExprGroupFuncBase::getString (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getString not implemented"); }
  MArray<Bool> TableExprGroupFuncBase::getArrayBool (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getArrayBool not implemented"); }
  MArray<Int64> TableExprGroupFuncBase::getArrayInt (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getArrayInt not implemented"); }
  MArray<Double> TableExprGroupFuncBase::getArrayDouble (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getArrayDouble not implemented"); }
  MArray<DComplex> TableExprGroupFuncBase::getArrayDComplex (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getArrayDComplex not implemented"); }
  MArray<MVTime> TableExprGroupFuncBase::getArrayDate (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getArrayDate not implemented"); }
  MArray<String> TableExprGroupFuncBase::getArrayString (const vector<TableExprId>&)
  { throw TableInvExpr ("TableExprGroupFuncBase::getArrayString not implemented"); }


  TableExprGroupNull::TableExprGroupNull (TableExprNodeRep* node)
    : TableExprGroupFuncBase (node)
  {}
  TableExprGroupNull::~TableExprGroupNull()
  {}
  Bool TableExprGroupNull::isLazy() const
    { return True; }
  void TableExprGroupNull::apply (const TableExprId&)
  {
    throw TableInvExpr ("TableExprGroupFunc::apply should not be called for "
                        " lazy aggregation");
  }

  TableExprGroupFirst::TableExprGroupFirst (TableExprNodeRep* node)
    : TableExprGroupFuncBase (node)
  {}
  TableExprGroupFirst::~TableExprGroupFirst()
  {}
  void TableExprGroupFirst::apply (const TableExprId& id)
  {
    // Keep first one.
    if (itsId.rownr() < 0) {
      itsId = id;
    }
  }
  Bool TableExprGroupFirst::getBool (const vector<TableExprId>&)
    { return itsOperand->getBool (itsId); }
  Int64 TableExprGroupFirst::getInt (const vector<TableExprId>&)
    { return itsOperand->getInt (itsId); }
  Double TableExprGroupFirst::getDouble (const vector<TableExprId>&)
    { return itsOperand->getDouble (itsId); }
  DComplex TableExprGroupFirst::getDComplex (const vector<TableExprId>&)
    { return itsOperand->getDComplex (itsId); }
  MVTime TableExprGroupFirst::getDate (const vector<TableExprId>&)
    { return itsOperand->getDate (itsId); }
  String TableExprGroupFirst::getString (const vector<TableExprId>&)
    { return itsOperand->getString (itsId); }
  MArray<Bool> TableExprGroupFirst::getArrayBool (const vector<TableExprId>&)
    { return itsOperand->getArrayBool (itsId); }
  MArray<Int64> TableExprGroupFirst::getArrayInt (const vector<TableExprId>&)
    { return itsOperand->getArrayInt (itsId); }
  MArray<Double> TableExprGroupFirst::getArrayDouble (const vector<TableExprId>&)
    { return itsOperand->getArrayDouble (itsId); }
  MArray<DComplex> TableExprGroupFirst:: getArrayDComplex (const vector<TableExprId>&)
    { return itsOperand->getArrayDComplex (itsId); }
  MArray<MVTime> TableExprGroupFirst::getArrayDate (const vector<TableExprId>&)
    { return itsOperand->getArrayDate (itsId); }
  MArray<String> TableExprGroupFirst::getArrayString (const vector<TableExprId>&)
    { return itsOperand->getArrayString (itsId); }

  TableExprGroupLast::TableExprGroupLast (TableExprNodeRep* node)
    : TableExprGroupFirst (node)
  {}
  TableExprGroupLast::~TableExprGroupLast()
  {}
  void TableExprGroupLast::apply (const TableExprId& id)
  {
    itsId = id;
  }

  TableExprGroupExprId::TableExprGroupExprId (TableExprNodeRep* node)
    : TableExprGroupFuncBase (node)
  {
    itsIds = new vector<TableExprId>();
  }
  TableExprGroupExprId::~TableExprGroupExprId()
  {}
  Bool TableExprGroupExprId::isLazy() const
  {
    return True;
  }
  void TableExprGroupExprId::apply (const TableExprId& id)
  {
    itsIds->push_back (id);
  }
  CountedPtr<vector<TableExprId> > TableExprGroupExprId::getIds() const
  {
    return itsIds;
  }

  TableExprGroupRowid::TableExprGroupRowid (TableExprNodeRep* node)
    : TableExprGroupFuncBase (node)
  {}
  TableExprGroupRowid::~TableExprGroupRowid()
  {}
  Bool TableExprGroupRowid::isLazy() const
  {
    return True;
  }
  void TableExprGroupRowid::apply (const TableExprId&)
  {
    throw TableInvExpr ("TableExprGroupRowid::apply should not be called");
  }
  MArray<Int64> TableExprGroupRowid::getArrayInt (const vector<TableExprId>& ids)
  {
    Vector<Int64> rowIds(ids.size());
    for (size_t i=0; i<ids.size(); ++ i) {
      rowIds[i] = ids[i].rownr();
    }
    return MArray<Int64>(rowIds);  }

  TableExprGroupAggr::TableExprGroupAggr (TableExprNodeRep* node)
    : TableExprGroupFuncBase (node)
  {}
  TableExprGroupAggr::~TableExprGroupAggr()
  {}
  Bool TableExprGroupAggr::isLazy() const
  {
    return True;
  }
  void TableExprGroupAggr::apply (const TableExprId&)
  {
    throw TableInvExpr ("TableExprGroupAggr::apply should not be called");
  }
  MArray<Bool> TableExprGroupAggr::getArrayBool (const vector<TableExprId>& ids)
    { return getArray<Bool>(ids); }
  MArray<Int64> TableExprGroupAggr::getArrayInt (const vector<TableExprId>& ids)
    { return getArray<Int64>(ids); }
  MArray<Double> TableExprGroupAggr::getArrayDouble (const vector<TableExprId>& ids)
    { return getArray<Double>(ids); }
  MArray<DComplex> TableExprGroupAggr::getArrayDComplex (const vector<TableExprId>& ids)
    { return getArray<DComplex>(ids); }
  MArray<MVTime> TableExprGroupAggr::getArrayDate (const vector<TableExprId>& ids)
    { return getArray<MVTime>(ids); }
  MArray<String> TableExprGroupAggr::getArrayString (const vector<TableExprId>& ids)
    { return getArray<String>(ids); }


  TableExprGroupFuncBool::~TableExprGroupFuncBool()
  {}
  Bool TableExprGroupFuncBool::getBool (const vector<TableExprId>&)
    { return itsValue; }

  TableExprGroupFuncInt::~TableExprGroupFuncInt()
  {}
  Int64 TableExprGroupFuncInt::getInt (const vector<TableExprId>&)
    { return itsValue; }
  Double TableExprGroupFuncInt::getDouble (const vector<TableExprId>&)
    { return itsValue; }

  TableExprGroupFuncDouble::~TableExprGroupFuncDouble()
  {}
  Double TableExprGroupFuncDouble::getDouble (const vector<TableExprId>&)
    { return itsValue; }

  TableExprGroupFuncDComplex::~TableExprGroupFuncDComplex()
  {}
  DComplex TableExprGroupFuncDComplex::getDComplex (const vector<TableExprId>&)
    { return itsValue; }

  TableExprGroupFuncString::~TableExprGroupFuncString()
  {}
  String TableExprGroupFuncString::getString (const vector<TableExprId>&)
    { return itsValue; }


  TableExprGroupFuncArrayBool::~TableExprGroupFuncArrayBool()
  {}
  MArray<Bool> TableExprGroupFuncArrayBool::getArrayBool (const vector<TableExprId>&)
    { return MArray<Bool>(itsValue); }
  Bool TableExprGroupFuncArrayBool::checkShape (const MArrayBase& arr,
                                                const String& func)
  {
    if (itsValue.empty()) {
      itsValue.resize (arr.shape(), arr.hasMask());
      return True;    // first time itsValue is used
    }
    if (! itsValue.shape().isEqual (arr.shape())) {
      throw TableInvExpr ("Mismatching array shapes in aggregate function " +
                          func);
    }
    AlwaysAssert (arr.hasMask() == itsValue.hasMask(), AipsError);
    return False;
  }

  TableExprGroupFuncArrayInt::~TableExprGroupFuncArrayInt()
  {}
  MArray<Int64> TableExprGroupFuncArrayInt::getArrayInt (const vector<TableExprId>&)
    { return MArray<Int64>(itsValue); }
  Bool TableExprGroupFuncArrayInt::checkShape (const MArrayBase& arr,
                                               const String& func)
  {
    if (itsValue.empty()) {
      itsValue.resize (arr.shape(), arr.hasMask());
      return True;    // first time itsValue is used
    }
    if (! itsValue.shape().isEqual (arr.shape())) {
      throw TableInvExpr ("Mismatching array shapes in aggregate function " +
                          func);
    }
    AlwaysAssert (arr.hasMask() == itsValue.hasMask(), AipsError);
    return False;
  }

  TableExprGroupFuncArrayDouble::~TableExprGroupFuncArrayDouble()
  {}
  MArray<Double> TableExprGroupFuncArrayDouble::getArrayDouble (const vector<TableExprId>&)
    { return MArray<Double>(itsValue); }
  Bool TableExprGroupFuncArrayDouble::checkShape (const MArrayBase& arr,
                                                  const String& func)
  {
    if (itsValue.empty()) {
      itsValue.resize (arr.shape(), arr.hasMask());
      return True;    // first time itsValue is used
    }
    if (! itsValue.shape().isEqual (arr.shape())) {
      throw TableInvExpr ("Mismatching array shapes in aggregate function " +
                          func);
    }
    AlwaysAssert (arr.hasMask() == itsValue.hasMask(), AipsError);
    return False;
  }

  TableExprGroupFuncArrayDComplex::~TableExprGroupFuncArrayDComplex()
  {}
  MArray<DComplex> TableExprGroupFuncArrayDComplex::getArrayDComplex (const vector<TableExprId>&)
    { return MArray<DComplex>(itsValue); }
  Bool TableExprGroupFuncArrayDComplex::checkShape (const MArrayBase& arr,
                                                    const String& func)
  {
    if (itsValue.empty()) {
      itsValue.resize (arr.shape(), arr.hasMask());
      return True;    // first time itsValue is used
    }
    if (! itsValue.shape().isEqual (arr.shape())) {
      throw TableInvExpr ("Mismatching array shapes in aggregate function " +
                          func);
    }
    AlwaysAssert (arr.hasMask() == itsValue.hasMask(), AipsError);
    return False;
  }

  TableExprGroupFuncArrayDate::~TableExprGroupFuncArrayDate()
  {}
  MArray<MVTime> TableExprGroupFuncArrayDate::getArrayDate (const vector<TableExprId>&)
    { return MArray<MVTime>(itsValue); }
  Bool TableExprGroupFuncArrayDate::checkShape (const MArrayBase& arr,
                                                const String& func)
  {
    if (itsValue.empty()) {
      itsValue.resize (arr.shape(), arr.hasMask());
      return True;    // first time itsValue is used
    }
    if (! itsValue.shape().isEqual (arr.shape())) {
      throw TableInvExpr ("Mismatching array shapes in aggregate function " +
                          func);
    }
    AlwaysAssert (arr.hasMask() == itsValue.hasMask(), AipsError);
    return False;
  }

  TableExprGroupFuncArrayString::~TableExprGroupFuncArrayString()
  {}
  MArray<String> TableExprGroupFuncArrayString::getArrayString (const vector<TableExprId>&)
    { return MArray<String>(itsValue); }
  Bool TableExprGroupFuncArrayString::checkShape (const MArrayBase& arr,
                                                  const String& func)
  {
    if (itsValue.empty()) {
      itsValue.resize (arr.shape(), arr.hasMask());
      return True;    // first time itsValue is used
    }
    if (! itsValue.shape().isEqual (arr.shape())) {
      throw TableInvExpr ("Mismatching array shapes in aggregate function " +
                          func);
    }
    AlwaysAssert (arr.hasMask() == itsValue.hasMask(), AipsError);
    return False;
  }


  TableExprGroupFuncSet::TableExprGroupFuncSet
  (const vector<TableExprNodeRep*>& aggrNodes)
    : itsId (0)
  {
    itsFuncs.reserve (aggrNodes.size());
    for (uInt i=0; i<aggrNodes.size(); ++i) {
      itsFuncs.push_back (aggrNodes[i]->makeGroupAggrFunc());
      itsFuncs[i]->setSeqnr (i);
    }
  }

  void TableExprGroupFuncSet::add
  (const CountedPtr<TableExprGroupFuncBase>& func)
  {
    size_t seqnr = itsFuncs.size();
    itsFuncs.push_back (func);
    itsFuncs[seqnr]->setSeqnr (seqnr);
  }

  void TableExprGroupFuncSet::apply (const TableExprId& id)
  {
    itsId = id;
    for (uInt i=0; i<itsFuncs.size(); ++i) {
      itsFuncs[i]->apply (id);
    }
  }


} //# NAMESPACE CASACORE - END
