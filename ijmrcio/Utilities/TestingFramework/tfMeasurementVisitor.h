#ifndef MEASUREMENTVISITOR_H 	/* -*- c++ -*- */
#define MEASUREMENTVISITOR_H


//  $Id: MeasurementVisitor.h,v 1.3 2003-12-05 20:46:58 blowek1 Exp $
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
/// \version $Revision: 1.3 $
/// \brief  
///
///

#include <iostream>

namespace testutil {
  

  // foward decalarions so we don't have to include Measurement.h
  class Measurement;
  class MeasurementFile;
  class DataMeasurement;
  class TextData;
  class PlainText;
  class StringText;
  class NumericData;
  class IntegerNumeric;
  class FloatNumeric;
  class DoubleNumeric;
  class BooleanNumeric;
  

  /// \brief class for the visitor design pattern for Measurements
  ///
  /// The default versions of the functions do nothing. The childern
  /// visitor function alls the parent's visitor function.
  class MeasurementVisitor {
  public:
    virtual ~MeasurementVisitor(void) = 0;

    virtual void Visit(Measurement &m);

    virtual void Visit(MeasurementFile &m);

    virtual void Visit(DataMeasurement &m);

    virtual void Visit(TextData &m);

    virtual void Visit(PlainText &m);

    virtual void Visit(StringText &m);

    virtual void Visit(NumericData &m);

    virtual void Visit(IntegerNumeric &m);

    virtual void Visit(FloatNumeric &m);

    virtual void Visit(DoubleNumeric &m);

    virtual void Visit(BooleanNumeric &m);

  };


  /// \brief Visitor to print out the differences between measurements
  ///
  /// This visitor will be called from the generated test data and
  /// compared with the ground truth input data.
  ///
  /// \note This may modifiy both Measurement objects.
  class DifferenceVisitor 
    : public MeasurementVisitor {
  public:

    virtual ~DifferenceVisitor(void);

    /// \brief sets the input measurement
    ///
    /// This function does not free the measurement. And the
    /// measurement object must be valid when the visitation takes place.
    virtual void SetBaselineMeasurement(Measurement *m);

    /// \brief set the output stream
    virtual void SetOutStream(std::ostream &_os);

    using MeasurementVisitor::Visit;

    /// \brief prints out the input and the test measurements
    ///
    /// by default this will be called for all types of measurements
    /// unless overridden
    virtual void Visit(Measurement &m);
    
    /// \brief performs a "diff" on the two texticies and prints
    virtual void Visit(PlainText &m);


    virtual void Visit(MeasurementFile &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(DataMeasurement &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(TextData &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(StringText &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(NumericData &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(IntegerNumeric &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(FloatNumeric &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(DoubleNumeric &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(BooleanNumeric &m) { this->MeasurementVisitor::Visit(m); }

  protected:
    Measurement *_baseline; // not freed
    
    /// \brief get the out put stream
    virtual std::ostream &GetOutStream(void) const;

    /// \brief modifies the Measuremnt objects to add
    virtual void AddPrefixesToNames(Measurement &test, Measurement &input);

  private:
    
    std::ostream *os; 
    
  };


  /// \brief Visitor to compare two measurements
  ///
  /// This visitor will be called from the generated test data and
  /// compared with the ground truth input data.
  ///
  /// \note This may modifiy both Measurement objects.
  class CompareVisitor 
    : public MeasurementVisitor {
  public:
    CompareVisitor(void);
    virtual ~CompareVisitor(void);

    //@{
    /// \brief set/get tolerance for compareing numeric data
    virtual void SetToleranceOff(void) { this->tolerantCompare = false; }
    virtual void SetTolerance( double tolerance );
    virtual double GetTolerance( void ) const;
    //@}

    /// \brief returns the the results of the comparison
    ///
    /// return true if the visited measurement and input measurement
    /// are equivalent
    ///
    /// \pre is is assumed that visitation has already occoured,
    /// otherwise the results are undefined
    virtual bool GetResult(void) const {return this->compareResults;}

    /// \brief sets the input measurement
    ///
    /// This function does not free the measurement. And the
    /// measurement object must be valid when the visitation takes place.
    virtual void SetBaselineMeasurement(Measurement *m);

 
    /// \brief uses the default operator== of the measurements
    virtual void Visit(Measurement &m);        

    /// \brief equivalent comparison but adds relative tolerance options
    virtual void Visit(NumericData &m);

    virtual void Visit(MeasurementFile &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(DataMeasurement &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(TextData &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(PlainText &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(StringText &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(IntegerNumeric &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(FloatNumeric &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(DoubleNumeric &m) { this->MeasurementVisitor::Visit(m); }
    virtual void Visit(BooleanNumeric &m) { this->MeasurementVisitor::Visit(m); }
    

  protected:
    Measurement *_baseline; // do not free
    
    bool compareResults;
    bool tolerantCompare;
    double relativeTolerance;
  };

}


#endif // MEASUREMENTVISITOR_H
    
