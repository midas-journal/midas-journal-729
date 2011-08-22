//  $Id: MeasurementVisitor.cpp,v 1.5 2003-12-12 20:20:52 blowek1 Exp $
// ===========================================================================
//
//                            PUBLIC DOMAIN NOTICE
//            Office of High Performance Computing and Communications
//
//  This software/database is a "United States Government Work" under the
//  terms of the United States Copyright Act.  It was written as part of
//  the author's official duties as a United States Government employee and
//  thus cannot be copyrighted.  This software is freely available
//  to the public for use. The National Library of Medicine and the U.S.
//  Government have not placed any restriction on its use or reproduction.
//
//  Although all reasonable efforts have been taken to ensure the accuracy
//  and reliability of the software and data, the NLM and the U.S.
//  Government do not and cannot warrant the performance or results that
//  may be obtained by using this software or data. The NLM and the U.S.
//  Government disclaim all warranties, express or implied, including
//  warranties of performance, merchantability or fitness for any particular
//  purpose.
//
//  Please cite the author in any work or product based on this material.
//
// ===========================================================================
//

/// \file
/// \author  Bradley Lowekamp
/// \version $Revision: 1.5 $
/// \brief  
///
///

#include "tfMeasurementVisitor.h"
#include "tfMeasurement.h"
#include "tfDiff.h"
#include <sstream>

namespace testutil {


  MeasurementVisitor::~MeasurementVisitor(void) {}

  void MeasurementVisitor::Visit(Measurement &m) {}

  void MeasurementVisitor::Visit(MeasurementFile &m) {
    this->Visit(static_cast<Measurement&>(m));
  }

  void MeasurementVisitor::Visit(DataMeasurement &m) {
    this->Visit(static_cast<Measurement&>(m));
  }   

  void MeasurementVisitor::Visit(TextData &m) {
    this->Visit(static_cast<DataMeasurement&>(m));
  }

  void MeasurementVisitor::Visit(PlainText &m) {
    this->Visit(static_cast<TextData&>(m));
  }

  void MeasurementVisitor::Visit(StringText &m) {
    this->Visit(static_cast<TextData&>(m));
  }
  
  void MeasurementVisitor::Visit(NumericData &m) {
    this->Visit(static_cast<DataMeasurement&>(m));
  }
  
  void MeasurementVisitor::Visit(IntegerNumeric &m) {
    this->Visit(static_cast<NumericData&>(m));
  }
  
  void MeasurementVisitor::Visit(FloatNumeric &m)  {
    this->Visit(static_cast<NumericData&>(m));
  }

  void MeasurementVisitor::Visit(DoubleNumeric &m)  {
    this->Visit(static_cast<NumericData&>(m));
  }
  
  void MeasurementVisitor::Visit(BooleanNumeric &m)  {
    this->Visit(static_cast<NumericData&>(m));
  }

  DifferenceVisitor::~DifferenceVisitor(void) {}
  
  void DifferenceVisitor::SetBaselineMeasurement(Measurement *m) { this->_baseline = m;}
  
  void DifferenceVisitor::SetOutStream(std::ostream &_os) { this->os = &_os;}
    
  void DifferenceVisitor::Visit(Measurement &m) {
    Measurement &baseline = *this->_baseline; 
    
    this->AddPrefixesToNames(m, baseline);
    
    this->GetOutStream() << m << std::endl;
    this->GetOutStream() << baseline << std::endl;
  }


  
  void DifferenceVisitor::Visit(PlainText &m) {
    Measurement &baseline = *this->_baseline; 
    
    std::ostringstream ostr;
    Diff(baseline.GetContent(), m.GetContent(), ostr);
    PlainText diff;
    diff.SetAttributeName(std::string("Difference ")+m.GetAttributeName());
    diff.SetContent(ostr.str());
    
    this->GetOutStream() << diff << std::endl;
  }
  
  std::ostream &DifferenceVisitor::GetOutStream(void) const {
    return *this->os;
  }

  void DifferenceVisitor::AddPrefixesToNames(Measurement &test, Measurement &baseline) {
    
    baseline.SetAttributeName(std::string("Baseline ") + baseline.GetAttributeName());
    test.SetAttributeName(std::string("Test ") + test.GetAttributeName());
  }


  CompareVisitor::CompareVisitor(void) :
    compareResults(false),
    tolerantCompare(false),
    relativeTolerance(0.0) 
  {
  }

  CompareVisitor::~CompareVisitor(void) {}

  void CompareVisitor::SetTolerance( double tolerance )
  {
    if (tolerance == 0.0) 
      this->SetToleranceOff();
    else 
      {
      this->tolerantCompare = true;
      this->relativeTolerance = tolerance;
      }
  }

  double CompareVisitor::GetTolerance( void ) const
  {
    if (this->tolerantCompare)
      return this->relativeTolerance;
    else
      return 0.0;
  }

  void CompareVisitor::SetBaselineMeasurement(Measurement *m) { this->_baseline = m;}
  

  void CompareVisitor::Visit(Measurement &m) {    
    Measurement &baseline = *this->_baseline;
    this->compareResults = ( baseline == m );
  }

  void CompareVisitor::Visit(NumericData &m) {
    NumericData *baseline;
    if ( (baseline = dynamic_cast<NumericData*>(this->_baseline))) 
      {
      if ( this->GetTolerance() != 0.0 )
        this->compareResults = baseline->IsEqualTolerant(m, this->GetTolerance() );
      else 
        this->compareResults = ( *baseline == m );
      }
    else
      {
      this->compareResults = false;
      }
  }

}

