//  $Id: RegressionTest.cpp,v 1.17 2005-06-23 19:14:39 blowek1 Exp $
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
/// \version $Revision: 1.17 $
/// \brief  
///
///
#include "tfRegressionTest.h"
#include "tfMeasurement.h"
#include "tfMeasurementVisitor.h"
#include "expat.h"
#include <string>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <new>
#include <typeinfo>
#include <ctime>
#include <fstream>
#include <assert.h>
#include <list>
#include <limits>
#include <cstring>
#if !defined(WIN32) || !defined(_WIN32)
#include <unistd.h>
#endif

#define CATCH_STDEXCEPTION(type) catch( type &e) { std::cerr << #type ": \"" << e.what() << "\"\n";}

namespace testutil {

  
  RegressionTest::RegressionTest(void) : 
    os(0), 
    is(0), 
    relativeTolerance(std::numeric_limits<float>::epsilon()*64), 
    inputUnmatched(0),
    expectedReturn(0) {
  }

  RegressionTest::~RegressionTest(void) {
    this->ReadXMLInputFree();
  }
  
  
void RegressionTest::SetInFileSearchPath(const fileutil::PathList &pathList) {
    this->searchPath = pathList;
  }


void RegressionTest::PrependInFileSearchPath(const fileutil::PathName &path) {
    this->searchPath.push_front( path );
  }

const fileutil::PathList &RegressionTest::GetInFileSearchPath(void) const {
    return this->searchPath;
  }
      
  void RegressionTest::SetInFileName(const std::string &fname) {
    this->inFileName = fileutil::PathName(fname); 
  }
  
   
  const std::string &RegressionTest::GetInFileName(void) const {
    return this->inFileName.GetPathName();
  }

  void RegressionTest::SetOutFileName(const std::string &fname) {
    this->outFileName = fileutil::PathName(fname); 
  }
  
  const std::string &RegressionTest::GetOutFileName(void) const {
    return this->outFileName.GetPathName();
  }

  int RegressionTest::GetExpectedReturn(void) const {
    return this->expectedReturn;
  }

  void RegressionTest::SetExpectedReturn( int _expectedReturn ) {
    this->expectedReturn = _expectedReturn;
  }

  void RegressionTest::SetRelativeTolerance(double tol) {
    this->relativeTolerance = tol;
  }
  
  double RegressionTest::GetRelativeTolerance(void) const {
    return this->relativeTolerance;
  }

  std::ostream &RegressionTest::GetOutStream(void) const {
    return this->os ? *this->os : std::cout;
  }

  std::istream &RegressionTest::GetInStream(void) const {
    return this->is ? *this->is : std::cin;
  }

  bool RegressionTest::GetCompareMode(void) const {
    if (this->is)
      return true;
    if (this->GetInFileName() == "")
      return false;
    fileutil::PathName infname = this->inFileName.SearchPathList(this->GetInFileSearchPath());
    if (infname.GetPathName() != "")
      return infname.IsFile() && infname.IsReadable();
    else 					   
      return this->inFileName.IsFile() && this->inFileName.IsReadable();
  }

 

  void RegressionTest::BeginMeasurement(void) {
    
    
    // open up the output, or use std::cout
    if (this->GetOutFileName() != "") {
      this->os = new std::ofstream(this->GetOutFileName().c_str());
      if (!this->os) {
	throw std::bad_alloc();
      }
      if (!*this->os) { 
	delete this->os;
	this->os = 0; 
	throw std::runtime_error("unable to open output file \"" + this->GetOutFileName() + "\"!");
      }
    } 

    if (this->GetInFileName() != "") {
      std::string infname = this->inFileName.SearchPathList(this->GetInFileSearchPath()).GetNativePathName();
      if (infname == "")
	infname = this->GetInFileName();
      this->is = new std::ifstream(infname.c_str(), std::ios::binary);
      if (!this->is) {
	throw std::bad_alloc();
      }
      if (!*this->is) {
	delete this->is;
	this->is = 0; 
	throw std::runtime_error("unable to open input file \"" + this->GetInFileName() + "\"!");
      }
    } 

    if (!this->GetCompareMode()) {
      this->WriteXMLOutputStart();
    } else {
      this->ReadXMLInput();
    }
  }
    
  void RegressionTest::EndMeasurement(void) {
    
    if (!this->GetCompareMode()) 
      this->WriteXMLOutputEnd();
    else
      this->ReadXMLInputFree();

    if (this->os) {
      delete this->os;
      this->os = 0;
    }

    if (this->is) {
      delete this->is;
      this->is = 0;
    }

    
    
  }


  void RegressionTest::ParseAndRemoveArguments(int &argc, char *argv[]) {
    const char *infname = 0;
    const char *outfname = 0;
	
    // search for the arguments that we know and remove them
	
    for (int i = 1; i < argc;) 
      {      
      // output filename option
      if (!strcmp("-O", argv[i])) 
        {
	if (i < argc-1) 
          {
	  outfname = argv[i+1];
          fileutil::PathName pn(outfname);
	  if (pn.IsFile() && !pn.IsWritable()) 
            {
	    std::cerr << "unable to write to file \"" << outfname << "\"!" << std::endl;
	    outfname = 0;
            }

	  for (int j = i; j+2 < argc; ++j)
	    std::swap(argv[j+2], argv[j]);
	  argc -=2;
          }
        }    
      // input filename option
      else if (!strcmp("-I", argv[i])) 
        {
	if (i < argc-1) 
          {
	  
	  infname = argv[i+1];	 
          fileutil::PathName pn(infname);
	  if (!pn.IsFile() || !pn.IsReadable()) 
            {	  
            throw std::runtime_error(std::string("unable to read input file \"") + infname + "\"!");
 	    infname = 0;
            } 

	  for (int j = i; j+2 < argc; ++j)
	    std::swap(argv[j+2], argv[j]);
	  argc -=2;
          }
        }
      // expected return value
      else if (!strcmp("-R", argv[i]))
        {
        const char* charExpectedReturn;
        if (i < argc-1) 
          {	  
	  charExpectedReturn = argv[i+1];
          int num = (int)strtol(  argv[i+1],  0, 10);
	  
          this->SetExpectedReturn(num);
	  for (int j = i; j+2 < argc; ++j)
	    std::swap(argv[j+2], argv[j]);
	  argc -=2;
          }
        }
      // additional in file search path
      else if (!strcmp("-P", argv[i]))
        {

	if (i < argc-1) 
          {	  
	  const char *insearchpath = argv[i+1];	 
          fileutil::PathName pn(insearchpath);
	  if (!pn.IsDirectory() || !pn.IsReadable()) 
            {	  
            throw std::runtime_error(std::string("unable to read input path \"") + insearchpath + "\"!");
 	    infname = 0;
            } 
     
	  for (int j = i; j+2 < argc; ++j)
	    std::swap(argv[j+2], argv[j]);
	  argc -=2;
          this->PrependInFileSearchPath( pn );
          }
        }
      else
        {
        break;
        }
      
      }

    if (infname)
      this->SetInFileName(infname);
    if (outfname)
      this->SetOutFileName(outfname);
  }

  int RegressionTest::Main(int argc, char *argv[]) {
	  

    int ret = -1; 

    std::set_unexpected(&_UnexpectedHandler);
    std::set_terminate(&_TerminateHandler);


    try {
      this->inputUnmatched++;
      
      this->ParseAndRemoveArguments(argc, argv);
      

      this->BeginMeasurement();

      
      ret = this->Test(argc, argv);


      this->EndMeasurement();

      this->inputUnmatched--;
    } 
    CATCH_STDEXCEPTION(std::length_error)
      CATCH_STDEXCEPTION(std::domain_error)
      CATCH_STDEXCEPTION(std::out_of_range)
      CATCH_STDEXCEPTION(std::invalid_argument)
      CATCH_STDEXCEPTION(std::bad_alloc)
      CATCH_STDEXCEPTION(std::bad_cast)
      CATCH_STDEXCEPTION(std::range_error)
      CATCH_STDEXCEPTION(std::overflow_error)
      CATCH_STDEXCEPTION(std::underflow_error)
      CATCH_STDEXCEPTION(std::runtime_error)
      CATCH_STDEXCEPTION(std::logic_error)
      CATCH_STDEXCEPTION(std::exception)
      catch(...) {
	std::cerr << "unknow exception" << std::endl;
      }
    if ( ret != this->GetExpectedReturn() )
      {
      return ret;
      }
    else if ( this->GetCompareMode() )
      {
      return this->inputUnmatched;
      }
    else 
      {
      return EXIT_SUCCESS;
      }

  }

  void RegressionTest::UnmatchedMeasurement(Measurement &test){    
    this->inputUnmatched++;
     test.SetAttributeName(std::string("Unmatched ") + test.GetAttributeName());
    this->GetOutStream() << test << std::endl;
  }

  void RegressionTest::MatchedMeasurement(Measurement &test) { 
    //    std::cout << "matched measurement \"" << test.GetAttributeName() << "\"." << std::endl;    
  }

  void RegressionTest::DifferenceMeasurement(Measurement &test, Measurement &input)  {  
    //    std::cout << "difference measurement \"" << test.GetAttributeName() << "\"." << std::endl;  
    this->inputUnmatched++;
    DifferenceVisitor *vis = this->CreateDifferenceVisitor();
    vis->SetOutStream(this->GetOutStream());
    vis->SetBaselineMeasurement(&input);
    test.AcceptVisitor(*vis);
    delete vis;
  }
      

  DifferenceVisitor *RegressionTest::CreateDifferenceVisitor(void) const {
    return new DifferenceVisitor;
  }

  int RegressionTest::CompareMeasurement(Measurement &test, bool tolerant) {
    // check to see if the top if the Inputs is the same type and name
    Measurement *baseline;
    int ret = 1;
    if (!this->Input.empty()) {
      
      baseline = this->Input.front();
      this->Input.pop_front();
      
      if (baseline->GetAttributeName() == test.GetAttributeName() &&
	  baseline->GetAttributeType() == test.GetAttributeType()) {
	
	// this is so very ugly
	// use the path of the test file to find the input file
	MeasurementFile *inf, *tf;
	if ((inf = dynamic_cast<MeasurementFile*>(baseline)) &&
	    (tf = dynamic_cast<MeasurementFile*>(&test))) {
        
        // search the path list for the baseline or input file name
        fileutil::PathName pn = fileutil::PathName(inf->GetFileName()).Tail().SearchPathList( this->GetInFileSearchPath() );        
        if ( pn.GetPathName() != "" )
          {
          // set it if we found it
          inf->SetFileName( pn.GetPathName() );
          }
        
        
	}
	
	 
        CompareVisitor *vis = this->CreateCompareVisitor();
        vis->SetBaselineMeasurement(baseline);

	if ( tolerant )
          vis->SetTolerance( this->GetRelativeTolerance() );
        else 
          vis->SetToleranceOff();
        
        test.AcceptVisitor(*vis);

        if ( vis->GetResult() )
          {
          // the results matched
          ret = 1;
          this->MatchedMeasurement(test);
          }
        else 
          {
          // the results differ
          ret = 0;
          this->DifferenceMeasurement(test, *baseline);
          }
        
        delete vis;
        
      } else 
	this->UnmatchedMeasurement(test);
      delete baseline;
    } else 
      this->UnmatchedMeasurement(test);

    return ret;
  }


  CompareVisitor *RegressionTest::CreateCompareVisitor(void) const {
    return new CompareVisitor;
  }

  int RegressionTest::MeasurementTextString(const std::string &str, const std::string &name) {
    StringText st;
    st.SetAttributeName(name);
    st.SetContent(str);

    if (this->GetCompareMode()) {
      return this->CompareMeasurement(st);
    } else 
      this->GetOutStream() << st << std::endl;
    return 0;
  }

  
  int RegressionTest::MeasurementTextPlain(const std::string &str, const std::string &name) {
    PlainText st;
    st.SetAttributeName(name);
    st.SetContent(str);
    
    if (this->GetCompareMode()) {
      return this->CompareMeasurement(st);
    } else 
      this->GetOutStream() << st << std::endl;
    return 0;
  }
  
  int RegressionTest::MeasurementFileTextPlain(const std::string &fileName, const std::string &name)  {
    MeasurementFile mf;
    mf.SetAttributeType("text/plain");
    mf.SetAttributeName(name);

    if (this->GetCompareMode()) {
      mf.SetFileName(fileName);
      return this->CompareMeasurement(mf);
    } else {
    mf.SetFileName(fileutil::PathName(fileName).Tail().GetPathName());
      this->GetOutStream() << mf << std::endl;
    }    
    return 0;
  }

  

//   int RegressionTest::MeasurementLinkURL(const std::string &name) {
//     return -1;
//   }

//   int RegressionTest::MeasurementLinkImage(const std::string &name) {
//     return -1;
//   }

 
  int RegressionTest::MeasurementNumericInteger(long i, const std::string &name, bool tolerant) {
    IntegerNumeric in;
    in.SetAttributeName(name);
    in.SetValue(i);
    
    if (this->GetCompareMode()) {
      return this->CompareMeasurement(in,tolerant);
    } else {
      this->GetOutStream() << in << std::endl;
    }    
    return 0;
  }

  int RegressionTest::MeasurementNumericFloat(float f, const std::string &name, bool tolerant) {
    FloatNumeric fn;
    fn.SetAttributeName(name);
    fn.SetValue(f);
    
    if (this->GetCompareMode()) {
	return  this->CompareMeasurement(fn,tolerant);
    } else {
      this->GetOutStream() << fn << std::endl;
    }    
    return 0;
   
  }

  int RegressionTest::MeasurementNumericDouble(double d, const std::string &name, bool tolerant) {
    DoubleNumeric dn;
    dn.SetAttributeName(name);
    dn.SetValue(d);
    
    if (this->GetCompareMode()) {
      return this->CompareMeasurement(dn,tolerant);
    } else {
      this->GetOutStream() << dn << std::endl;
    }    
    return 0;
  }

  int RegressionTest::MeasurementNumericBoolean(bool b, const std::string &name, bool tolerant) {
    BooleanNumeric bn;
    bn.SetAttributeName(name);
    bn.SetValue(b);
    
    if (this->GetCompareMode()) {
      return this->CompareMeasurement(bn,tolerant);
    } else {
      this->GetOutStream() << bn << std::endl;
    }    
    return 0;
  }
    
//   int RegressionTest::MeasurementImagePNG(const std::string &name) {
//     return -1;
//   }

  
//   int RegressionTest::MeasurementImageJPEG(const std::string &name) {
//     return -1;
//   }

  
  int RegressionTest::MeasurementFileImagePNG(const std::string &fileName, const std::string &name)  {
    MeasurementFile mf;
    mf.SetAttributeType("image/png");
    mf.SetAttributeName(name);

    if (this->GetCompareMode()) {
      mf.SetFileName(fileName);
      return this->CompareMeasurement(mf);
    } else {
    mf.SetFileName(fileutil::PathName(fileName).Tail().GetPathName());
    this->GetOutStream() << mf << std::endl;
    }    
    return 0;
  }

  
  int RegressionTest::MeasurementFileImageJPEG(const std::string &fileName, const std::string &name) {
    MeasurementFile mf;
    mf.SetAttributeType("image/jpeg");
    mf.SetAttributeName(name);
    
    if (this->GetCompareMode()) {
      mf.SetFileName(fileName);
      return this->CompareMeasurement(mf);
    } else {
    mf.SetFileName(fileutil::PathName(fileName).Tail().GetPathName());
      this->GetOutStream() << mf << std::endl;
    }    
    return 0;
  }

  
      
      
  void RegressionTest::WriteXMLOutputStart(void) {
    char hostname[256];
    time_t t;
    time(&t);    
#if defined(WIN32)
    strcpy(hostname, "unknow windows host");
#else
    gethostname(hostname, 256);
#endif
    this->GetOutStream() << "<?xml version=\"1.0\" encoding=\"US-ASCII\"?>" << std::endl;
    this->GetOutStream() << "<!-- created on "  << hostname << " at " <<ctime(&t) << " -->" << std::endl;
    this->GetOutStream() << "<output>" << std::endl;
  }

      
  void RegressionTest::WriteXMLOutputEnd(void) {
    this->GetOutStream() << "</output>" << std::endl;
  }

namespace {
    

    struct XMLParseData {
      XMLParseData(void) : depth(0), m(0) {}
      ~XMLParseData(void) { assert(!m);}
      int depth;
      std::string data;
      Measurement *m;
      std::list<Measurement*> measurements;
    };


    void start(void *data, const char *el, const char **attr) {  
      XMLParseData &pd = *reinterpret_cast<XMLParseData*>(data);
      
      assert(!pd.m);
      assert(pd.data.length() == 0);

      if (pd.depth == 0) {
	if (el && std::string(el) != "output" ) {
	  // error, expected output
	  std::cerr << "XML: expected \"output\" document element type" << std::endl;
	}	
      } else if (pd.depth == 1) {
	
	if (std::string(el) == "DartMeasurement") {	  
	  DataMeasurement *dm = 0;

	  
	  for (int i = 0; attr[i]; i += 2) {
	    if (std::string(attr[i]) == "type") {
	      std::string str = attr[i+1];
	      dm = CreateDataMeasurement(attr[i+1]);
	      if (!dm) {
		std::cerr << "XML: unknow dart measurement type \"" << attr[i+1] << "\"" << std::endl;		
		dm = new PlainText;
	      }
	    }
	  }
	  pd.m = dm;
		
	  // get what we know, but we can't really set any of these yet
	  for (int i = 0; attr[i]; i += 2) {
	    std::string str = attr[i];
	    if (str == "name") 
	      dm->SetAttributeName(attr[i+1]);
	    else if (str == "type") 
	      ; // nothing to do with it now
	    else if (str == "encoding")
	      dm->SetAttributeEncoding(attr[i+1]);
 	    else if (str == "compression")
 	      dm->SetAttributeCompression(attr[i+1]);
	    else 
	      std::cerr << "XML: unknow attribute \"" << str << "\"" << std::endl;
	  }
	  

	} else if (std::string(el) == "DartMeasurementFile") {
	  MeasurementFile *mf = new MeasurementFile;
	  pd.m = mf;
	  
	  // some default
	  mf->SetAttributeType("text/plain"); 
	  mf->SetAttributeName("unknown");
	  
	  // get what we know
	  for (int i = 0; attr[i]; i += 2) {
	    std::string str = attr[i];
	    if (str == "name") 
	      mf->SetAttributeName(attr[i+1]);
	    else if (str == "type")
	      mf->SetAttributeType(attr[i+1]);
	    else 
	      std::cerr << "XML: unknow attribute \"" << str << "\"" << std::endl;
	  }
	  

	} else {
	  // error, unknow element type
	  std::cerr << "XML: unknown element type \"" << el << "\"" << std::endl;
	}
      } else {	
	std::cerr << "XML: depth is too deep" << std::endl;
      }

      ++pd.depth;
    }
    
    void end(void *data, const char *el) { 
      XMLParseData &pd = *reinterpret_cast<XMLParseData*>(data); 

      if (pd.m && pd.depth == 2) {
	//	pd.data += '\0';
	pd.m->SetContent(pd.data);	
	pd.measurements.push_back(pd.m);
      } else if (pd.m) {
	// may not be posible, but let's make sure we keep things clean
	delete pd.m;
      }

      pd.m = 0;
      
      pd.data.resize(0);

      --pd.depth;
    }
            
    void data(void *data, const char *s, int len) {    
      XMLParseData &pd = *reinterpret_cast<XMLParseData*>(data);
      
      if (pd.depth == 2) 
	pd.data.append(s, len);
      
    }
}

  void RegressionTest::ReadXMLInput(void) {
    static const size_t BUFFSIZE = 1024;
    char *buff;
    XML_Parser parser;
    std::istream::pos_type l, m, size;
    XMLParseData pd;

    
   
    // get the size of the file
    l = this->GetInStream().tellg();
    this->GetInStream().seekg(0, std::ios::end);
    m = this->GetInStream().tellg();
    size = m-l;
    this->GetInStream().seekg(0, std::ios::beg);


    parser = XML_ParserCreate("US-ASCII");

    if (!parser)       
      throw std::bad_alloc();
    
    XML_SetElementHandler(parser, start, end);

    XML_SetCharacterDataHandler(parser, data);
    
    XML_SetUserData(parser, &pd);
    
    while (true) {

      bool done;
      int len;
      //      std::istream::pos_type oldPos = this->GetInStream().tellg();
      
      if (!(buff = static_cast<char*>(XML_GetBuffer(parser, BUFFSIZE))))
	throw std::bad_alloc();
      
      this->GetInStream().read(buff, BUFFSIZE);

      if (!this->GetInStream().eof() && !this->GetInStream())
	throw std::runtime_error("XML:read error");

      if ((done = this->GetInStream().eof()))
	len = size%BUFFSIZE;
      else 
	len = BUFFSIZE;
      
      if (!XML_ParseBuffer(parser, len, done)) 	{
	std::cerr <<  "XML:Parse error at line " << XML_GetCurrentLineNumber(parser)
		  << ": \"" << XML_ErrorString(XML_GetErrorCode(parser)) << "\"\n";
	throw std::runtime_error("parse error");
      }

      if (done)
	break;
    }

    XML_ParserFree(parser);
    this->Input.swap(pd.measurements);
  }
  
  
  void RegressionTest::ReadXMLInputFree(void) {

    for (std::list<Measurement*>::iterator i = this->Input.begin(); i != this->Input.end(); ++i)
      delete *i;
    this->Input.resize(0);
  }

  void RegressionTest::_UnexpectedHandler(void) {
    std::cerr << "unexpected error!\n";
  }
      
  void RegressionTest::_TerminateHandler(void) {
    std::cerr << "terminated!\n";
  }

      
	
}
