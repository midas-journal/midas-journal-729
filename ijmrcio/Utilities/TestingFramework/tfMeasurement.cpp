//  $Id: Measurement.cpp,v 1.14 2004-05-14 19:44:14 blowek1 Exp $
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
/// \version $Revision: 1.14 $
/// \brief  
///
///

#include "tfMeasurement.h"
#include "tfMeasurementVisitor.h"
#include "tfBase64.h"
#include <ios>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <assert.h>
#include <cstring>

using namespace std;

namespace testutil {
  
  
  const std::string IntegerNumeric::type("numeric/integer");
  const std::string FloatNumeric::type("numeric/float");
  const std::string DoubleNumeric::type("numeric/double");
  const std::string BooleanNumeric::type("numeric/boolean");

  const std::string StringText::type("text/string");

  const std::string PlainText::type("text/plain");

  const std::string DataMeasurement::defaultEncoding("none");
  const std::string DataMeasurement::defaultCompression("none");
  const std::string DataMeasurement::measurementElementTypeName("DartMeasurement");

  
  const std::string MeasurementFile::measurementElementTypeName("DartMeasurementFile");

  DataMeasurement *CreateDataMeasurement(const std::string attributeType) {
    if (attributeType == "text/plain") 
      return new PlainText;
    else if (attributeType == "text/string") {
      return new StringText;
    } else if (attributeType == "numeric/integer") {
      return new IntegerNumeric;
    } else if (attributeType == "numeric/float") {
      return new FloatNumeric;
    } else if (attributeType == "numeric/double") {
      return new DoubleNumeric;
    } else if (attributeType == "numeric/boolean") {
      return new BooleanNumeric;
    } 
    return 0;
  }
  
  Measurement::Measurement(void) : applyEscapeSequences(true) {}

  Measurement::~Measurement(void) {}

  void Measurement::SetAttributeName(const std::string &n) {
    this->name = n;
  }

  Measurement::Measurement(const Measurement &m ) :name(m.name), applyEscapeSequences(m.applyEscapeSequences) {
    
  }
  
  Measurement &Measurement::operator=(const Measurement &m) {
    this->name = m.name;
    this->applyEscapeSequences = m.applyEscapeSequences;
    return *this;
  }

  const std::string &Measurement::GetAttributeName(void) const {
    return this->name;
  }

  bool Measurement::GetApplyEscapeSequences(void) const {
    return this->applyEscapeSequences;
  }

  void Measurement::SetApplyEscapeSequences(bool b) {
    this->applyEscapeSequences = b;
  }

  void Measurement::ApplyEscapeSequencesOn(void) {
    this->SetApplyEscapeSequences(true);
  }

  void Measurement::ApplyEscapeSequencesOff(void) {
    this->SetApplyEscapeSequences(false);
  }
    
  std::string Measurement::ApplyEscapeSequences(const std::string &str) const {
      
    std::string::size_type j = 0;
    std::string::size_type i = str.find_first_of("&<>");
    if (i == str.length())
      return str;
    std::string result;
    while (i < str.length()) {
      result.append(str,j,i-j);
      if (str[i] == '&')
	result.append("&amp;");
      else if (str[i] == '<')
	result.append("&lt;");
      else if (str[i] == '>')
	result.append("&gt;"); 
	
      j = i + 1;
      i = str.find_first_of("&<>", j);
    }      
    result.append(str,j,i-j);
    return result;
  }


  void Measurement::Print(std::ostream &os) const {
    this->PrintStartTag(os);
    this->PrintContent(os);
    this->PrintEndTag(os);
  }

  
  bool Measurement::operator==(Measurement &m) {
    return this->IsEqual(m);
  }

  
  void Measurement::AcceptVisitor(MeasurementVisitor &visitor) {
    visitor.Visit(*this);
  }
  

  void Measurement::PrintContent(std::ostream &os) const {
    if (this->GetApplyEscapeSequences()) {
      os << this->ApplyEscapeSequences(this->GetContent());
    } else {	
      os << this->GetContent();
    }
      
  }

  void Measurement::PrintEndTag(std::ostream &os) const {
    os << "</" << this->GetElementTypeName() << ">";
  }
      
  std::ostream &operator<<(std::ostream &os, const Measurement &m) {
    m.Print(os);
    return os;
  }
    


  MeasurementFile::~MeasurementFile(void) {};
    
    
   MeasurementFile::MeasurementFile(const MeasurementFile &m) :Measurement(m) , type(m.type), fileName(m.fileName) {
     
  }
    
  MeasurementFile & MeasurementFile::operator=(const MeasurementFile &m) {
    this->Measurement::operator=(m);
    this->type = m.type;
    this->fileName = m.fileName;
    return *this;
  }

  void MeasurementFile::SetAttributeType(const std::string &t) {
    this->type = t;
  }

  const std::string &MeasurementFile::GetAttributeType(void) const {
    return this->type;
  }

  const std::string &MeasurementFile::GetElementTypeName(void) const  {
    return this->measurementElementTypeName;
  }

  void MeasurementFile::SetFileName(const std::string &fname) {
    this->fileName = fname;
  }

  const std::string &MeasurementFile::GetFileName(void) const {
    return this->fileName;
  }    
    
  
  void MeasurementFile::SetContent(const std::string &fname) {
    this->SetFileName(fname);
  }

  const std::string &MeasurementFile::GetContent(void) const {
    return this->GetFileName();
  }

  
  void MeasurementFile::AcceptVisitor(MeasurementVisitor &visitor) {
    visitor.Visit(*this);
  }

  

  DataMeasurement *MeasurementFile::Load(void) {
    std::ios::openmode mode = std::ios::in;
    bool binary = this->GetAttributeType().substr(0, 4) != "text";

    // if the file is not text then open in binary mode
    if (binary)
      mode |= std::ios::binary;// this actually doesn't matter since we read with read

    std::ifstream is(this->GetFileName().c_str(), mode);
    std::ifstream::pos_type l, m, size;
    char *buff;

    if (!is) {
      std::cerr << "error opening file \"" << this->GetFileName() << "\"." << std::endl;
      return new PlainText();
    }
      

    // get the size of the file
    l = is.tellg();
    is.seekg(0, std::ios::end);
    m = is.tellg();
    size = m-l;
    is.seekg(0, std::ios::beg);


    buff = static_cast<char*>(malloc(size));
    if (!buff)
      throw std::bad_alloc();

    if (binary) {
      is.read(buff, size);
    } else {
      assert(sizeof(char) == 1);
      // slow way to read but need the conversion, and it may be better than rescanning the input for getline
      char *tbuff = buff;
      char c;
      size = 0;
      while (is.get(c)) {
	*tbuff++ = c;
	size += 1;
      }	
    }

    DataMeasurement *dm = CreateDataMeasurement(this->GetAttributeType());
    if (!dm) {
      std::cerr << "Unknown measurement type \"" << this->GetAttributeType() << "\"." << std::endl;
      dm = new PlainText();
    }
    dm->SetContentAsData(buff, size);
    dm->SetAttributeName(this->GetAttributeName());


    char c=0;
    is.get(c);	// this should cause and EOF

    ios_base::iostate state = is.exceptions();
    if (state & ios_base::badbit) cerr << "throws for bad";
    if (state & ios_base::failbit) cerr << "throws for fail";
    if (state & ios_base::eofbit) cerr << "throws for eof";

    if (!is.eof() || is.bad()) {
      throw std::runtime_error("failure reading file");
    }
    

    free(buff);
    return dm;
    
  }

  bool MeasurementFile::IsEqual(Measurement &m) {
    if (this->GetAttributeName() != m.GetAttributeName() ||
	this->GetAttributeType() != m.GetAttributeType())
      return false;
    DataMeasurement *dm = Load();
    bool v = (m == *dm); // switch so if the are both file we can do it again
    delete dm;

    // TODO
    //
    // load the file??
    // compare
    return v;
  }
    

  void MeasurementFile::PrintStartTag(std::ostream &os) const {
      
    os << "<" << this->GetElementTypeName() << " name=\"" << this->GetAttributeName() 
       << "\" type=\"" << this->GetAttributeType() << "\">";
  }
  
  

  
  DataMeasurement::~DataMeasurement(void) {
    this->DeleteContentAsData();
  }

  DataMeasurement::DataMeasurement(void) : data(0) {
    this->encoding = this->GetDefaultAttributeEncoding();
    this->compression = this->GetDefaultAttributeCompression();
  }

  DataMeasurement::DataMeasurement(const DataMeasurement &m) 
    : Measurement(m), encoding(m.encoding), compression(m.compression), data(0){
  }
    
  DataMeasurement &DataMeasurement::operator=(const DataMeasurement &m) {
    this->Measurement::operator=(m);
    this->data = 0;
    this->dataSize = 0;
    this->encoding = m.encoding;
    this->compression = m.compression;
    return *this;
  }
     
  const std::string &DataMeasurement::GetElementTypeName(void) const {
    return this->measurementElementTypeName;
  }
     

  void DataMeasurement::SetAttributeEncoding(const std::string &_encoding) {
    this->encoding = _encoding;
  }
     
  const std::string &DataMeasurement::GetAttributeEncoding(void) const {
    return this->encoding;
  }     

  const std::string &DataMeasurement::GetDefaultAttributeEncoding(void) const {
    return this->defaultEncoding;
  }
  
  void DataMeasurement::SetAttributeCompression(const std::string &_compression) {
    this->compression = _compression;
  }
     
  const std::string &DataMeasurement::GetAttributeCompression(void)const {
    return this->compression;
  }

  const std::string &DataMeasurement::GetDefaultAttributeCompression(void)const {
    return this->defaultCompression;
  }


  void DataMeasurement::UpdateContentAsData(void) {
    this->DeleteContentAsData();
    if (this->GetAttributeCompression() != "none") {
      std::cerr << "unsupported compression \"" << this->GetAttributeCompression() << "\" used." << std::endl;
      
    }

    if (this->GetAttributeEncoding() == "none") {
      // just use the content it'self
    } else if (this->GetAttributeEncoding() == "base64") {
      Base64 encoder;
      this->dataSize = encoder.IsBase64(this->GetContent());
      if (this->dataSize) {
	this->data = static_cast<void*>(malloc(this->dataSize));
	if (!this->data)
	  throw std::bad_alloc();
	encoder.Decode(this->GetContent(), this->data);
      } else {
	if (this->GetContent().length() != 0) 
	  std::cerr << "error decodeing base64" << std::endl;

      }
      
    } else { 
      std::cerr << "unsupported encoding \"" << this->GetAttributeEncoding() << "\" used." << std::endl;
    
    }

    

  }

  void DataMeasurement::DeleteContentAsData(void) {
    if (this->data)
      free(this->data);
    this->data = 0;
    this->dataSize = 0;
  }

  const void *DataMeasurement::GetContentAsData(void) {
    if (!this->data)
      this->UpdateContentAsData();
    if (this->data)
      return this->data;
    else 
      return this->GetContent().c_str();
  }

  size_t DataMeasurement::GetContentAsDataSize(void) {
    if (!this->data)
      this->UpdateContentAsData();
    if (!this->data)
      return this->GetContent().length();
    else 
      return this->dataSize;
  }

  void DataMeasurement::SetContentAsData(const void *_data, size_t size) {
    this->DeleteContentAsData();
    this->data = malloc(size);

    if (!this->data) 
      throw std::bad_alloc();
    this->dataSize = size;

    memcpy(this->data, _data, size);
    
    // encode the data to be moved into the content
    if (this->GetAttributeCompression() != "none") {
      std::cerr << "unsupported compression \"" << this->GetAttributeCompression() << "\" used." << std::endl;      
      this->DeleteContentAsData();
    }

    if (this->GetAttributeEncoding() == "none") {
      
    } else if (this->GetAttributeEncoding() == "base64") {
      Base64 encoder;
      
      this->SetContent(encoder.Encode(this->data, this->dataSize));
      
    } else { 
      std::cerr << "unsupported encoding \"" << this->GetAttributeEncoding() << "\" used." << std::endl;
    
    }

  }

  
  void DataMeasurement::AcceptVisitor(MeasurementVisitor &visitor) {
    visitor.Visit(*this);
  }


  bool DataMeasurement::IsEqual(Measurement &m) {
    
    DataMeasurement *td = dynamic_cast<DataMeasurement*>(&m);
    
    if (!td)
      return m == *this; // if the other side is a file let them handle it
    else if (this->GetAttributeName() == td->GetAttributeName() &&
	     this->GetAttributeType() == td->GetAttributeType() && 
	     this->GetContentAsDataSize() == td->GetContentAsDataSize()) {
      return !memcmp(this->GetContentAsData(), td->GetContentAsData(), this->GetContentAsDataSize());
    } else 
      return false;
  }

     
  void DataMeasurement::PrintStartTag(std::ostream &os) const {
    os << "<" << this->GetElementTypeName() << " name=\"" << this->GetAttributeName() 
       << "\" type=\"" << this->GetAttributeType() << "\"";

    if (this->GetAttributeEncoding() != this->GetDefaultAttributeEncoding())
      os << " encoding=\"" << this->GetAttributeEncoding() << "\"";
    if (this->GetAttributeCompression() != this->GetDefaultAttributeCompression()) {
      os << " compression=\"" << this->GetAttributeCompression() << "\"";
    }
    os << ">";
  }
   

  TextData::~TextData(void) {
  }

  TextData::TextData(const TextData &m) : DataMeasurement(m), content(m.content) {
  }

  TextData &TextData::operator=(const TextData &m) {
    this->DataMeasurement::operator=(m);
    this->content = m.content;
    return *this;
  }
    
  void TextData::AcceptVisitor(MeasurementVisitor &visitor) {
    visitor.Visit(*this);
  }


  

  void PlainText::AcceptVisitor( MeasurementVisitor &visitor) {
    visitor.Visit(*this);
  }
  
  
  void StringText::AcceptVisitor( MeasurementVisitor &visitor) {
    visitor.Visit(*this);
  }

  
  
  void NumericData::AcceptVisitor(MeasurementVisitor &visitor) {
    visitor.Visit(*this);
  }


  void IntegerNumeric::AcceptVisitor(MeasurementVisitor &visitor) {
    visitor.Visit(*this);
  }
 
  int IntegerNumeric::GetValue(void) const {
    std::istringstream iss(this->GetContent());
    int v;
    iss >> v;
    return v;    
  }

  void IntegerNumeric::SetValue(int v) {
    std::ostringstream oss;
    oss << v;
    this->SetContent(oss.str());
  }

  void FloatNumeric::AcceptVisitor(MeasurementVisitor &visitor) {
    visitor.Visit(*this);
  }
 
  float FloatNumeric::GetValue(void) const {
    std::istringstream iss(this->GetContent());
    float v;
    iss >> v;
    return v;    
  }

  void FloatNumeric::SetValue(float v) {
    std::ostringstream oss;    
    oss.precision(7);
    oss << v;
    this->SetContent(oss.str());
  }

  void DoubleNumeric::AcceptVisitor(MeasurementVisitor &visitor) {
    visitor.Visit(*this);
  }
 
  double DoubleNumeric::GetValue(void) const {
    std::istringstream iss(this->GetContent());
    double v;
    iss >> v;
    return v;    
  }

  void DoubleNumeric::SetValue(double v) {
    std::ostringstream oss;
    oss.precision(16);
    oss << v;
    this->SetContent(oss.str());
  }
 
  void BooleanNumeric::AcceptVisitor(MeasurementVisitor &visitor) {
    visitor.Visit(*this);
  }
 
  bool BooleanNumeric::GetValue(void) const {
    std::istringstream iss(this->GetContent());
    bool v;
    iss >> v;
    return v;    
  }

  void BooleanNumeric::SetValue(bool v) {
    std::ostringstream oss;
    oss << v;
    this->SetContent(oss.str());
  }
}
