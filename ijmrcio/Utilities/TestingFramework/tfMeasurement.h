#ifndef MEASUREMENT_H 	/* -*- c++ -*- */
#define MEASUREMENT_H


//  $Id: tfMeasurement.h,v 1.1 2009/09/23 19:53:27 blowekamp Exp $
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
/// \version $Revision: 1.1 $
/// \brief  classes to be used as measurements for a dart board testing
///
/// \note the cache mechnism and conversion methods are not efficient
/// and are waitful with allocations and copying.

#include <string> 
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <numeric>
#include <limits>

namespace testutil {
  
  // forward declarations
  class MeasurementVisitor;
  class DataMeasurement;

  /// \brief Abstract base class for all measurements
  /// 
  /// \note uses default copy contructor and assignment operator
  ///
  /// \todo all the measuement calls should not throw any exceptions,
  /// to aid in debugging when errors do occour, additially all error
  /// messages generating by the testing framework, should be labled
  /// as coming from it 
  class Measurement {
  public:

    /// Default constructor
    Measurement(void);

    virtual ~Measurement(void);

    Measurement(const Measurement &);

    Measurement &operator=(const Measurement &);

    /// \brief return string of the element type for the tag
    virtual const std::string &GetElementTypeName(void) const = 0;

    //@{
    /// \brief set/get the value for the attribute "name"
    virtual void SetAttributeName(const std::string &n);
    virtual const std::string &GetAttributeName(void) const;
    //@}

    /// \brief get the value for the attribute "type"
    virtual const std::string &GetAttributeType(void) const = 0;



    //@{ 
    /// \brief set/get wether escape sequences are applied
    ///
    /// Before any value or charactor data is printed,
    /// ApplyEscapeSequences will be applies.
    virtual bool GetApplyEscapeSequences(void) const;
    virtual void SetApplyEscapeSequences(bool b);
    virtual void ApplyEscapeSequencesOn(void);
    virtual void ApplyEscapeSequencesOff(void);
    //@}
    
    /// \brief converts all needed charactors to escape sequences
    ///
    /// any occurence of &<> is replaces with &amp;,&lt;,&gt; respectfully
    virtual std::string ApplyEscapeSequences(const std::string &str) const;
    //virtual std::string ReplaceEscapeSequences(const std::string &str);
    
    
    /// \brief prints the measurement in XML format    
    virtual void Print(std::ostream &os) const;

    //@}
    /// \brief set/get the contents as a string
    ///
    /// this is the raw contents, with out escape sequences
    virtual void SetContent(const std::string &content) = 0;
    virtual const std::string &GetContent(void) const = 0;
    //@}


    /// provided as a convient call too IsEqual
    virtual bool operator==(Measurement &m);

    
    /// \brief helper function for comparisons
    virtual bool IsEqual(Measurement &) = 0;

    /// \brief Visitor function
    ///
    /// This is part of the double dispatch mechinisim of the visitor
    /// design pattern. This function should be reimplemented in all
    /// derived classes
    virtual void AcceptVisitor(MeasurementVisitor &visitor);

  protected:

    /// \brief helper function to print the starting XML tag
    virtual void PrintStartTag(std::ostream &os) const = 0;

    /// \brief helper function to print the content of the tag
    ///
    /// by default will print Content which may have the escape
    /// sequences applied
    virtual void PrintContent(std::ostream &os) const;

    /// \brief helper function to print the ending XML tag
    /// 
    /// by default this will print </ElementTypeName>
    virtual void PrintEndTag(std::ostream &os) const;


    
  private:

    // name attribute
    std::string name;
    
    bool applyEscapeSequences;

   };


    
  /// \brief A Measurement where the content is a file name
  //
  //  \todo attention needs to be made to this class
  class MeasurementFile
    : public Measurement {
  public:
    
    MeasurementFile(void) {}

    virtual ~MeasurementFile(void);

    MeasurementFile(const MeasurementFile &m);
    
    MeasurementFile &operator=(const MeasurementFile &m);
    
    //@{
    /// \brief set/get the value for the attribute "type"
    ///
    /// what are the suported values?
    virtual  void SetAttributeType(const std::string &t);
    virtual const std::string &GetAttributeType(void) const ;
    //@}

    
    // see Measurement
    virtual const std::string &GetElementTypeName(void) const ;

    //@{
    /// \brief set/get the name of the file for the contents
    ///
    /// \todo the search path for this loading is undefined!
    virtual void SetFileName(const std::string &fname);
    virtual const std::string &GetFileName(void) const;
    //@}
    
    
    // see Measurement
    virtual void SetContent(const std::string &content);
    virtual const std::string &GetContent(void) const;

    // see Measurement
    virtual void AcceptVisitor(MeasurementVisitor &visitor);

    
    /// Loads this file into memory and tries to compare them again
    virtual bool IsEqual(Measurement &m);
    
  protected:

    // see Measurement
    virtual void PrintStartTag(std::ostream &os) const;

    /// \brief loads the file into a DataMeasurement
    ///
    /// This function loads the data into memory and the copies it
    /// into the DataMeasurement object. 
    ///
    /// \note the user must delete this object with delete to free the memory.
    DataMeasurement *Load(void);

    /// \brief comparison
    ///

  private:
    static const std::string measurementElementTypeName;
    std::string type;
    std::string fileName;
  };

  
  /// \brief A abstract Measurement type where the content is stored
  ///
  /// This class adds the functionality of compression and encoding,
  /// as well as storeing and convering the data.
  ///
  /// \note the ContentAsData is not fast, and will prodice extra
  /// copies of the data in memory
  class DataMeasurement 
    : public Measurement {
  public:
    DataMeasurement(void);
    virtual ~DataMeasurement(void);
     
    DataMeasurement(const DataMeasurement &m);
    
    DataMeasurement &operator=(const DataMeasurement &m);
     
    // see Measurement
    virtual const std::string &GetElementTypeName(void) const;
     

    //@{
    /// \brief set/get the encoding type
    ///
    /// only "none" and "base64 may besupported, and that will be determined
    /// by the specific type. By default this is "none".
    virtual void SetAttributeEncoding(const std::string &encoding);
    virtual const std::string &GetAttributeEncoding(void) const;
    virtual const std::string &GetDefaultAttributeEncoding(void) const;
    //@}
     
    //@{
    /// \brief set/get the compression
    ///
    /// only "none" and "gzip" may be supported. By default this is "none".
    ///
    /// \todo gzip is totally unimplemented
    virtual void SetAttributeCompression(const std::string &compression);
    virtual const std::string &GetAttributeCompression(void)const;
    virtual const std::string &GetDefaultAttributeCompression(void)const;
    //@}
    
    //@{
    /// \brief set/get the content as decoded/uncompressed bits
    ///
    /// Based on the current attributes the data is converted to/from
    /// the string based contents. If the compression or encoding is
    /// not supported then the data will be of lenght 0
    ///
    /// \todo this is where the compression is not supported
    /// \note this create extra copies of the data
    virtual const void *GetContentAsData(void);
    virtual size_t GetContentAsDataSize(void);
    virtual void SetContentAsData(const void *data, size_t size);
    //@}

    // see Measurement
    virtual void AcceptVisitor(MeasurementVisitor &visitor);

    /// \brief comparison
    ///
    /// Compares the ContentAsData. This frequently is not the best
    /// way forthis to be done and is reimplemented as needed
    virtual bool IsEqual(Measurement &m);
    
  protected:

    /// \brief updates the internal cache of the data 
    ///
    /// This function will be called when the content is needed in
    /// this format and will be called to update the data.
    void UpdateContentAsData(void);

    /// \brief free the internam cache of the data
    ///
    /// this function must be called when the contents changes!
    /// or else the cahce will become out of sync
    void DeleteContentAsData(void);

   

    // see Measurement
    virtual void PrintStartTag(std::ostream &os) const;
    
  private:
    
    static const std::string defaultEncoding;
    static const std::string defaultCompression;
    

    static const std::string measurementElementTypeName;
    
    std::string encoding; // type of encoding
    std::string compression; // type of compression

    
    size_t dataSize;
    void *data; // cache for the content as data
  };

  
  /// \brief Another abstract class for measurements of the text format
  class TextData 
    :public DataMeasurement {
  public:
    
    TextData(void) {}
    virtual ~TextData(void) = 0;
    TextData(const TextData &m);
    TextData &operator=(const TextData &m);
    

    // see Measurement
    virtual void SetContent(const std::string &str) {this->content = str;this->DeleteContentAsData();}    
    virtual const std::string &GetContent(void) const {return this->content;}

    // see Measurement
    virtual void AcceptVisitor(MeasurementVisitor &visitor);

  private:

    std::string content; // where the content is stored
  };

  /// \brief A concrete class for plain text
  ///
  /// Plain text is generally multiline longer blocks of charactors
  class PlainText 
    :public TextData {
  public:
    PlainText(void) {}
    virtual ~PlainText(void) {};
    PlainText(const PlainText &m) : TextData(m) {};
    PlainText &operator=(const PlainText &m) {
      this->TextData::operator=(m);
      return *this;
    }

    // see Measurement
    virtual const std::string &GetAttributeType(void) const {return this->type;}

    
    virtual void AcceptVisitor(MeasurementVisitor &visitor);

  private:
    static const std::string type;
  };

  
  /// \brief A concrete class for string text measurement
  ///
  /// String text is generally a short string type sequence
  class StringText 
    :public TextData {
  public:
    
    StringText(void) {}
    virtual ~StringText(void) {};
    StringText(const StringText &m) : TextData(m) {
    }
    StringText &operator=(const StringText &m) {
      this->TextData::operator=(m);
      return *this;
    } 

    // see Measurement
    virtual const std::string &GetAttributeType(void) const {return this->type;}

    
    // see Measurement
    virtual void AcceptVisitor(MeasurementVisitor &visitor);

  private:
    static const std::string type;
  };

  /// \brief Another abstract class for numeric measurements
  ///
  /// these classes "content" is text numbers
  class NumericData 
    :public DataMeasurement {
  public:
    NumericData(void) {}
    virtual ~NumericData(void) {};
    NumericData(const NumericData &m) : DataMeasurement(m), content(m.content) {
    }

    NumericData &operator=(const NumericData &m) {
      this->DataMeasurement::operator=(m);
      this->content = m.content;
      return *this;
    }

    // see Measurement
    virtual void SetContent(const std::string &str) {this->content = str;this->DeleteContentAsData();}    
    virtual const std::string &GetContent(void) const {return this->content;}

    // see Measurement
    virtual void AcceptVisitor(MeasurementVisitor &visitor);
    
    /// \brief is comparison with a percentage tollerance
    ///
    /// returns true of  the current value is with in tol percentage of m);
    ///
    ///  | u - v | <= e * |u| or |  u - v | <= e * |v|
    /// defines a "close with tolerance e" relationship between u and v
    ///
    /// Also it is check to see if the u an v are both less then
    /// epsilon for the type
    virtual bool IsEqualTolerant(const NumericData &rm, double tol) = 0;

  protected:

    template <class T>
    static bool _IsEqualTolerant(const T &lm, const T &rm, double tol) {
      tol = std::abs(tol);
      T temp = std::abs(lm - rm);     
      return  temp <= tol*std::abs(lm) ||
	temp <= tol*std::abs(rm) || 
	(std::abs(lm) < std::numeric_limits<T>::epsilon() &&
	 std::abs(rm) < std::numeric_limits<T>::epsilon());
    }
    
  private:

    std::string content; // where the content is stored

  };

  class IntegerNumeric
    : public NumericData {
  public:
    IntegerNumeric(void) {}
    virtual ~IntegerNumeric(void) {};
    IntegerNumeric(const IntegerNumeric &m) :NumericData(m) {
    }
    IntegerNumeric &operator=(const IntegerNumeric &m) {
      this->NumericData::operator=(m);
      return *this;
    }

    // see Measurement
    virtual const std::string &GetAttributeType(void) const {return this->type;}

    // see Measurement
    virtual void AcceptVisitor(MeasurementVisitor &visitor);
    
    int GetValue(void) const;
    void SetValue(int v); 

     // see NumericData
    virtual bool IsEqualTolerant(const NumericData &rm, double tol) {
      const IntegerNumeric *m = dynamic_cast<const IntegerNumeric*>(&rm);
      if (m)
	return _IsEqualTolerant(this->GetValue(), m->GetValue(), tol);
      else 
	return false;
    }



  private:
    static const std::string type;
    
  };

  class FloatNumeric
    : public NumericData {
  public:
    FloatNumeric(void) {}
    virtual ~FloatNumeric(void) {};
    FloatNumeric(const FloatNumeric &m) : NumericData(m) {      
    }
    FloatNumeric &operator=(const FloatNumeric &m) {
      this->NumericData::operator=(m);
      return *this;
    }

    // see Measurement
    virtual const std::string &GetAttributeType(void) const {return this->type;}

    // see Measurement
    virtual void AcceptVisitor(MeasurementVisitor &visitor);
    
    float GetValue(void) const;
    void SetValue(float v); 


    // see NumericData
    virtual bool IsEqualTolerant(const NumericData &rm, double tol) {
      const FloatNumeric *m = dynamic_cast<const FloatNumeric*>(&rm);
      if (m)
	return _IsEqualTolerant(this->GetValue(), m->GetValue(), tol);
      else 
	return false;
    }

  private:
    static const std::string type;
    
  };

   class DoubleNumeric
    : public NumericData {
  public:
     DoubleNumeric(void) {}
     virtual ~DoubleNumeric(void) {};
     DoubleNumeric(const DoubleNumeric &m) : NumericData(m) {
     }
     DoubleNumeric &operator=(const DoubleNumeric &m)  {
       this->NumericData::operator=(m);
       return *this;
     }

     // see Measurement
     virtual const std::string &GetAttributeType(void) const {return this->type;}

     // see Measurement
     virtual void AcceptVisitor(MeasurementVisitor &visitor);
    
     double GetValue(void) const;
     void SetValue(double v); 

     
     // see NumericData
     virtual bool IsEqualTolerant(const NumericData &rm, double tol) {
       const DoubleNumeric *m = dynamic_cast<const DoubleNumeric*>(&rm);
       if (m)
	 return _IsEqualTolerant(this->GetValue(), m->GetValue(), tol);
       else 
	return false;
    }

 

  private:
    static const std::string type;
    
  };


  class BooleanNumeric
    : public NumericData {
  public:
    BooleanNumeric(void) {}
    virtual ~BooleanNumeric(void) {};
    BooleanNumeric(const BooleanNumeric &m) :NumericData(m) {
    }
    BooleanNumeric &operator=(const BooleanNumeric &m) {
      this->NumericData::operator=(m);
      return *this;
    }

    // see Measurement
    virtual const std::string &GetAttributeType(void) const {return this->type;}

    // see Measurement
    virtual void AcceptVisitor(MeasurementVisitor &visitor);
    
    bool GetValue(void) const;
    void SetValue(bool v); 

    // see NumericData
    virtual bool IsEqualTolerant(const NumericData &rm, double tol) {
      const BooleanNumeric *m = dynamic_cast<const BooleanNumeric*>(&rm);
       if (m)
	 return this->GetValue() ==  m->GetValue();
       else 
	 return false;
    }


  private:
    static const std::string type;
    
  };



  /// helps with printing to streams
  std::ostream &operator<<(std::ostream &os, const Measurement &m);
  
  
  /// \brief A factory function based on attribute types
  ///
  /// Creates a concrete DataMeasurement which is of the has
  /// attributeType as it's attribute type with it handles.
  ///
  /// \note this is the limiting function for the expandibility of the
  /// measurement types. Every time a new type is created this class
  /// will need to be modified.
  /// \todo This should really implement some type of ObjectFactory so
  /// the types can be expanded.
  DataMeasurement *CreateDataMeasurement(const std::string attributeType);
}


#endif // MEASUREMENT_H
    
