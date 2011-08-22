
#include "itktfRegression.h"


#include "itkSize.h"
#include "itkIndex.h"
#include "itkImageRegion.h"
#include "itkPoint.h"
#include "itkVector.h"
#include "itkMatrix.h"

#include "itktfRegressionSupport.h"
#include "itktfCompareVisitor.h"
#include "itktfDifferenceVisitor.h"

namespace itk {

Regression::Regression(void)
{
  fileutil::PathList pathList = this->GetInFileSearchPath();
  pathList.push_front( fileutil::PathName( ITK_REGRESSION_DATA_BASELINE ) );
  this->SetInFileSearchPath(pathList);
  
  this->SetImageInsightToleranceOff();
  
  m_FileNameCount = 0;
}

Regression::~Regression(void) 
{
}

void Regression::SetImageInsightToleranceOff( void ) 
{ 
  this->SetImageInsightTolerance( 0.0, 0, 0 ); 
}

void Regression::SetImageInsightTolerance( double intensityTolerance, 
                               unsigned int numberOfPixelTolerance,
                               unsigned int radiusTolerance ) 
{
  m_IntensityTolerance = intensityTolerance;
  m_NumberOfPixelsTolerance = numberOfPixelTolerance;
  m_RadiusTolerance = radiusTolerance;
}


std::string Regression::GenerateFileName( std::string fileExtension, std::string rootPath ) const 
{
  std::string filename;

  uint16_t hash = 0xfb73; // arbitrary inital value
  std::string cmdline;

  // make one big string of all the arguments
  for (std::list<std::string>::const_iterator i = m_ArgumentList.begin(); i != m_ArgumentList.end(); ++i)
    {
    cmdline += *i;
    }
  
  // simple hash generating
  for ( size_t i = 0; i < cmdline.size(); ++i )
    {
    // rotate left 7
    hash = ( (hash<<7) | (hash >> (16-7)))  ^ cmdline[i];
    }
  

  std::ostringstream strstream;
  strstream << m_ArgumentList.front() << "_" << std::hex << hash << "_" << ++m_FileNameCount << "." << fileExtension;


  if ( rootPath != "" )
    {
    // append the root path to the genereate file name
    return fileutil::PathName( rootPath ).Append( fileutil::PathName( strstream.str() ) ).GetPathName();
      }
  else 
    {
    return strstream.str();
    }
}

int Regression::Main( int argc, char *argv[] ) 
{
  m_ArgumentList.clear();
  for ( int i = 0; i < argc; ++i) 
    {
    m_ArgumentList.push_back( std::string( argv[i] ) );
    }
  int ret = testutil::RegressionTest::Main( argc, argv );
  return ret;
}


int Regression::MeasurementInsightFileImage( const std::string &fileName, const std::string &name) 
{
  testutil::MeasurementFile mf;
  mf.SetAttributeType("image/itk");
  mf.SetAttributeName(name);
  
  if (this->GetCompareMode()) 
    {
    mf.SetFileName(fileName);
    return this->CompareMeasurement(mf);
    } 
  else 
    {
    mf.SetFileName(fileutil::PathName(fileName).Tail().GetPathName());
    
    this->GetOutStream() << mf << std::endl;
    }    
  return 0;
}


testutil::DifferenceVisitor *Regression::CreateDifferenceVisitor(void) const 
{
  itk::DifferenceVisitor * temp = new itk::DifferenceVisitor;
  temp->SetImageInsightTolerance( m_IntensityTolerance, m_NumberOfPixelsTolerance, m_RadiusTolerance );
  return temp;
}

testutil::CompareVisitor *Regression::CreateCompareVisitor(void) const {
  itk::CompareVisitor * temp = new itk::CompareVisitor;
  temp->SetImageInsightTolerance( m_IntensityTolerance, m_NumberOfPixelsTolerance, m_RadiusTolerance );
  return temp;
}
  
}
