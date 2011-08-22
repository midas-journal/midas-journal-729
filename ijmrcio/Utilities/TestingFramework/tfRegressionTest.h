#ifndef REGRESSIONTEST_H 	/* -*- c++ -*- */
#define REGRESSIONTEST_H


//  $Id: RegressionTest.h,v 1.16 2004-05-24 20:45:32 blowek1 Exp $
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
/// \version $Revision: 1.16 $
/// \brief  declaration of the RegressionTest testing framework class
///
///

#include "FileUtilities/PathName.h"
#include <string>
#include <iostream>
#include <list>
#include "TestingFramework/TestFrameworkConfigure.h"

#include <cstdlib>

namespace testutil {
  
  class Measurement;
  class DifferenceVisitor;
  class CompareVisitor;

  /// \brief A base class for implementing tests
  ///
  /// This class provides the functionality of being able help
  /// generate and compare measurements. It does this by operating in
  /// two modes, CompareMode, and non-CompareMode. In non-CompareMode
  /// the measurements are simply output to a file, in the other a
  /// file of measurements is loaded, called the baseline
  /// measurements, and then the measurements created, the test measuments, are
  /// compared, from this the compared output is generated.
  ///
  /// The command line arguments that it knows are -I infilename -O
  /// outfilename. If -I is not specified then the regression test
  /// will run in non-CompareMode
  /// 
  /// \todo The character set for strings need to be verified and
  /// printed correctly. Not printable characters are not handled
  /// correctly. 
  class TESTINGFRAMEWORK_EXPORT RegressionTest {
  public:

    RegressionTest(void);
    virtual ~RegressionTest(void);
      
    //@{
    /// \brief set/get the path to search for the input file
    ///
    /// The paths which are searched for the input files. The first one
    /// in the list to contain InFileName will be used.
    ///
    virtual void SetInFileSearchPath(const fileutil::PathList &pathList);
    virtual const fileutil::PathList &GetInFileSearchPath(void) const;
    //@}

    //@{
    /// \brief prepend path to the InFileSearchPath
    ///
    /// Prepends the path to the beging of the InFileSearchPath
    /// PathList 
    virtual void PrependInFileSearchPath(const fileutil::PathName &path);
    //@}
      
    //@{
    /// \brief set/get the input file name
    ///
    /// if the name is "" then it is considered not to be set, and no
    /// input will be used, there by putting the system into
    /// non-CompareMode 
    virtual void SetInFileName(const std::string &fname);
    virtual const std::string &GetInFileName(void) const;
    //@}

    //@{
    /// \brief set/get the output file name
    ///
    /// if the name is "" then it is considered not to be set and
    /// std::cout will be used for the output, and the output will
    /// conform to the expected output for Dart
    virtual void SetOutFileName(const std::string &fname);
    virtual const std::string &GetOutFileName(void) const;
    //@}

    //@{
    /// \brief set/get the expected return value from Test
    ///
    /// this value can also be set with the -R argument
    virtual int GetExpectedReturn(void) const;
    virtual void SetExpectedReturn( int expectedReturn );
    //@}

    //@{
    /// \brief set/get the relative tolerance of numeric values
    /// 
    /// Relative tolerance is used for numeric values. It is a maximum
    /// percentage error.
    virtual void SetRelativeTolerance(double tol);
    virtual double GetRelativeTolerance(void) const;
    ///@}

    /// \brief true if in CompareMode
    ///
    /// This check to see if the InfileName is openable or is already
    /// open, resultign the the measurements being in compare mode
    virtual bool GetCompareMode(void) const;

    /// \brief the main like funciton of the class
    ///
    /// This should be passed the command line arguments. It will the
    /// call ParseAndRemoveArguments to extract the input and output
    /// files. Next it will try to open them, read the input file, and then envoke Test
    /// with the remaining arguments. Finally the files will be
    /// closed.
    ///
    /// If in compare mode, the the return value is the number of
    /// measurements that did not match. Otherwise it is the return
    /// value of test.
    virtual int Main(int argc, char *argv[]);
      
  protected:

    /// \brief the function to perform the test and create measurments
    virtual int Test(int argc, char *argv[]) = 0;

    virtual int MeasurementTextString(const std::string &str, const std::string &name);
    virtual int MeasurementTextPlain(const std::string &str, const std::string &name);


//     virtual int MeasurementLinkURL(const std::string &name);
//    virtual int MeasurementLinkImage(const std::string &name);

    virtual int MeasurementNumericInteger(long i, const std::string &name, bool tolerant = false);
    virtual int MeasurementNumericFloat(float f, const std::string &name, bool tolerant = true);
    virtual int MeasurementNumericDouble(double d, const std::string &name, bool tolerant = true);
    virtual int MeasurementNumericBoolean(bool b, const std::string &name, bool tolerant = false);
    
//     virtual int MeasurementImagePNG(const std::string &name);
//     virtual int MeasurementImageJPEG(const std::string &name);



    ///@{
    /// \brief Records/Compares a measurement where the content is in a file
    ///
    /// Need to update this
    //
    /// The full pathname should be used for fileName, however this
    /// will not be checked. When in compare mode The full paths will
    /// be printed in the output, otherwise just the filename will be
    /// printed. When loading the files it is assumed that the "Baseline"
    /// file is in the same directory as the Test file name. (Search Path?)  
    /// 
    /// \todo The FileImages are not fully supported yet
    virtual int MeasurementFileTextPlain(const std::string &fileName, const std::string &name);
    virtual int MeasurementFileImagePNG(const std::string &fileName, const std::string &name);
    virtual int MeasurementFileImageJPEG(const std::string &fileName, const std::string &name);
    ///@}

         
    /// \brief gets the output stream
    ///
    /// Valid only when the files have been opened
    virtual std::ostream &GetOutStream(void) const;

    /// \brief gets the input stream
    ///
    /// Valid only when the files have been opened
    virtual std::istream &GetInStream(void) const;
    
    
    
    /// compares the test measurement with the current baseline
    /// measurement. May print output
    ///
    /// returns 0 if they are the same, non-zero other wise
    virtual int CompareMeasurement(Measurement &test, bool tolerant = false);

  private:
    /// opens and reads the files. called before Test
    virtual void BeginMeasurement(void);

    /// closes files, called after Test
    virtual void EndMeasurement(void);


    /// a test measurement did not match the current input
    /// measurement. May pring output
    virtual void UnmatchedMeasurement(Measurement &m);

    /// the test measurement matched and has the same value as the
    /// current input measurement. May print output
    virtual void MatchedMeasurement(Measurement &m);

    /// the test and the baseline measurement matched but they had
    /// different values. Then calles the DifferenceVisitor on the test
    virtual void DifferenceMeasurement(Measurement &test, Measurement &baseline);
    

    /// Creates a difference visitor for the measurement
    virtual DifferenceVisitor *CreateDifferenceVisitor(void) const;

    /// Creates a compare visitor for the measurement
    virtual CompareVisitor *CreateCompareVisitor(void) const;

    /// Pareses are removes the -I infilename -O outfilename from the
    /// begining of the command line arguments
    virtual void ParseAndRemoveArguments(int &argc, char *argv[]);

  
    /// writes the XML header when writing to a file
    virtual void WriteXMLOutputStart(void);

    /// writes the XML tail when writing to a file
    virtual void WriteXMLOutputEnd(void);
            

    /// Reads the InStream and creats the Input Measurements
    virtual void ReadXMLInput(void);

    /// Frees any data created
    virtual void ReadXMLInputFree(void);

    /// stores the input measuremnts
    std::list<Measurement*> Input;



  private:
      
    // these functions must be static
    static void _UnexpectedHandler(void);
    static void _TerminateHandler(void);

  protected:
    
    fileutil::PathName inFileName;
    fileutil::PathName outFileName;
    fileutil::PathList searchPath;

    std::ostream *os;
    std::istream *is;

    double relativeTolerance;

    int inputUnmatched;
    
    int expectedReturn;
    
  private: // private data

    RegressionTest(const RegressionTest &); // Not implemented
    RegressionTest &operator=(const RegressionTest &); // Not implemented

   
  };

      
	
}



#endif // RegressionsTest_H
    
