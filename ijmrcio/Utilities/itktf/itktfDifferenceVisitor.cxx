/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTestMain.h,v $
  Language:  C++
  Date:      $Date: 2009-03-03 15:09:43 $
  Version:   $Revision:  $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "itktfDifferenceVisitor.h"
#include "itktfRegressionSupport.h"


#include "itktfDifferenceImageFilter.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkImageFileWriter.h"

namespace itk {

DifferenceVisitor::DifferenceVisitor(void) 
{        
  this->SetImageInsightToleranceOff();
}

void DifferenceVisitor::SetImageInsightToleranceOff( void ) 
{ 
  this->SetImageInsightTolerance( 0.0, 0, 0 ); 
}

void DifferenceVisitor::SetImageInsightTolerance( double intensityTolerance, 
                                                  unsigned int numberOfPixelTolerance,
                                                  unsigned int radiusTolerance )
{
  m_IntensityTolerance = intensityTolerance;
  m_NumberOfPixelsTolerance = numberOfPixelTolerance;
  m_RadiusTolerance = radiusTolerance;
}

void DifferenceVisitor::Visit(testutil::MeasurementFile &m) 
{
  testutil::MeasurementFile *baseline;
  if ( m.GetAttributeType() == "image/itk" &&
       (baseline = dynamic_cast<testutil::MeasurementFile*>(this->_baseline)) )
    {
    DifferenceTestImage( m, 
                         *baseline, 
                         m_IntensityTolerance,
                         m_NumberOfPixelsTolerance, 
                         m_RadiusTolerance ) ;
    }
  else 
    {
    testutil::DifferenceVisitor::Visit(m);
    }
}

template< typename ImageType >
int ImageThing( typename ImageType::ConstPointer image, 
                const  fileutil::PathName &fileName, 
                const std::string &attributeName) 
{
  
  typedef unsigned char                  OutputPixelType;

  typedef itk::Image< typename ImageType::PixelType, 2 > ExtractImageType;

  typedef itk::Image<OutputPixelType, 2> OutputImageType;


  typedef itk::ExtractImageFilter<ImageType, ExtractImageType>                ExtractType;   
  typedef itk::RescaleIntensityImageFilter<ExtractImageType, OutputImageType> RescaleType;
  typedef itk::ImageFileWriter<OutputImageType>                               WriterType;
  typedef itk::ImageRegion<ImageType::ImageDimension>                         RegionType;


  typename ImageType::SizeType size; 
  size = image->GetLargestPossibleRegion().GetSize();
  
  //Get the center slice of the image,  In 3D, the first slice
  //is often a black slice with little debugging information.
  typename ImageType::IndexType index; 
  index.Fill(0);
  for (unsigned int i = 2; i < ImageType::ImageDimension; i++)
    {
    index[i]=size[i]/2;//NOTE: Integer Divide used to get
                       //approximately the center slice                       
    size[i] = 0;
    }

  
  RegionType region( index, size );
  std::cout << region;

  typename ExtractType::Pointer extract = ExtractType::New();
  extract->SetInput( image );
  extract->SetExtractionRegion( region );

  typename RescaleType::Pointer rescale = RescaleType::New();
  rescale->SetOutputMinimum( itk::NumericTraits<OutputPixelType>::NonpositiveMin() );
  rescale->SetOutputMaximum( itk::NumericTraits<OutputPixelType>::max() );
  rescale->SetInput( extract->GetOutput() );


    try
      {
      rescale->Update();
      }
    catch(const std::exception& e)
      {
      std::cerr << "Error during rescale of " << fileName.GetPathName() << std::endl;
      std::cerr << e.what() << "\n";
      }
    catch (...)
      {
      std::cerr << "Error during rescale of " << fileName.GetPathName() << std::endl;
      }

    try
      {

      
      typename WriterType::Pointer writer = WriterType::New();
      writer->SetInput( rescale->GetOutput() );
      writer->SetFileName( fileName.GetPathName() );
      writer->Update();
      }
    catch(const std::exception& e)
      {
      std::cerr << "Error during exctraction and writing of " << fileName.GetPathName() << std::endl;
      std::cerr << e.what() << "\n";
      }
    catch (...)
      {
      std::cerr << "Error during exctraction and writing of " << fileName.GetPathName() << std::endl;
      }

    testutil::MeasurementFile outputMeasurement;
    outputMeasurement.SetAttributeType("image/png");
    outputMeasurement.SetAttributeName("Difference " + attributeName );
    outputMeasurement.SetContent(  fileName.GetPathName() );
    std::cout << outputMeasurement << std::endl;

    return 0;  
}


// function was taken from itk's testing main
int DifferenceVisitor::DifferenceTestImage (testutil::MeasurementFile &testMeasurement,
                                            testutil::MeasurementFile &baselineMeasurement,
                                            double intensityTolerance,
                                            unsigned int numberOfPixelsTolerance, 
                                            unsigned int radiusTolerance )
{
  // Use the factory mechanism to read the test and baseline files and
  // convert them to double
  const unsigned int Dimension = ITK_TF_DIMENSION_MAX;
  typedef itk::Image<double, Dimension>        ImageType;
  typedef itk::ImageFileReader<ImageType>                  ReaderType;
  
  fileutil::PathName testImageFilename = testMeasurement.GetFileName();
  fileutil::PathName baselineImageFilename = baselineMeasurement.GetFileName();


  // Read the baseline file
  ReaderType::Pointer baselineReader = ReaderType::New();
  baselineReader->SetFileName(baselineImageFilename.GetPathName());
  
  // Read the file generated by the test
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName(testImageFilename.GetPathName());
  
  testReader->UpdateOutputInformation();
  baselineReader->UpdateOutputInformation();

  //////////////////////////////////////////////////////
  // The sizes of the baseline and test image must match to compare
  ImageType::SizeType baselineSize = baselineReader->GetOutput()->GetLargestPossibleRegion().GetSize();
  ImageType::SizeType testSize = testReader->GetOutput()->GetLargestPossibleRegion().GetSize();
  
  if (baselineSize != testSize)
    {
    // output two measurements of the image sizes'
    testutil::StringText statusMeasurement;
    std::ostringstream strstream;
    strstream << baselineSize;
    statusMeasurement.SetAttributeName("Baseline Size "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetContent( strstream.str() );
    std::cout << statusMeasurement << std::endl;

    strstream.str( "" );
    strstream << testSize;
    statusMeasurement.SetAttributeName("Test ImageSize "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetContent( strstream.str() );
    std::cout << statusMeasurement << std::endl;
    
    return 1;
    }
  
  //////////////////////////////////////////////////////
  // If the spacing is different just print extra measurements
  ImageType::SpacingType baselineSpacing = baselineReader->GetOutput()->GetSpacing();
  ImageType::SpacingType testSpacing = testReader->GetOutput()->GetSpacing();

  if (baselineSpacing != testSpacing)
    {
    // output two measurements of the image spacing
    testutil::StringText statusMeasurement;
    std::ostringstream strstream;
    strstream << baselineSpacing;
    statusMeasurement.SetAttributeName("Baseline Spacing "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetContent( strstream.str() );
    std::cout << statusMeasurement << std::endl;

    strstream.str( "" );
    strstream << testSpacing;
    statusMeasurement.SetAttributeName("Test Spacing "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetContent( strstream.str() );
    std::cout << statusMeasurement << std::endl;    
    }

  //////////////////////////////////////////////////////
  // If the origin is different just print extra measurements
  ImageType::PointType baselineOrigin = baselineReader->GetOutput()->GetOrigin();
  ImageType::PointType testOrigin = testReader->GetOutput()->GetOrigin();

  if (baselineOrigin != testOrigin)
    {
    // output two measurements of the image spacing
    testutil::StringText statusMeasurement;
    std::ostringstream strstream;
    strstream << baselineOrigin;
    statusMeasurement.SetAttributeName("Baseline Origin "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetContent( strstream.str() );
    std::cout << statusMeasurement << std::endl;

    strstream.str( "" );
    strstream << testOrigin;
    statusMeasurement.SetAttributeName("Test Origin "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetContent( strstream.str() );
    std::cout << statusMeasurement << std::endl;    
    }
  
  //////////////////////////////////////////////////////
  // Check to see if we have the same pixel type

  itk::ImageIOBase::IOPixelType baselinePixelType = baselineReader->GetImageIO()->GetPixelType();
  itk::ImageIOBase::IOPixelType testPixelType = testReader->GetImageIO()->GetPixelType();

  if ( baselinePixelType != testPixelType )
    {
    // output two measurements of the image pixel type
    testutil::StringText statusMeasurement;
    statusMeasurement.SetAttributeName("Baseline PixelType "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetContent( baselineReader->GetImageIO()->GetPixelTypeAsString( baselinePixelType ) );
    std::cout << statusMeasurement << std::endl;
    
    statusMeasurement.SetAttributeName("Test PixelType "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetContent( testReader->GetImageIO()->GetPixelTypeAsString( testPixelType ) );
    std::cout << statusMeasurement << std::endl;
    }

  itk::ImageIOBase::IOComponentType baselineComponentType = baselineReader->GetImageIO()->GetComponentType();
  itk::ImageIOBase::IOComponentType testComponentType = testReader->GetImageIO()->GetComponentType();

  if ( baselineComponentType != testComponentType )
    {
    // output two measurements of the image pixel type
    testutil::StringText statusMeasurement;
    statusMeasurement.SetAttributeName("Baseline ComponentType "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetContent( baselineReader->GetImageIO()->GetComponentTypeAsString( baselineComponentType ) );
    std::cout << statusMeasurement << std::endl;
    
    statusMeasurement.SetAttributeName("Test ComponentType "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetContent( testReader->GetImageIO()->GetComponentTypeAsString( testComponentType ) );
    std::cout << statusMeasurement << std::endl;
    }

  unsigned int baselineNumberOfComponents = baselineReader->GetImageIO()->GetNumberOfComponents();
  unsigned int testNumberOfComponents = testReader->GetImageIO()->GetNumberOfComponents();
  
  if ( baselineNumberOfComponents != testNumberOfComponents )
    {
    // output two measurements of the image pixel type
    testutil::IntegerNumeric statusMeasurement;
    statusMeasurement.SetAttributeName("Baseline Number Of Components "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetValue( baselineNumberOfComponents );
    std::cout << statusMeasurement << std::endl;
    
    statusMeasurement.SetAttributeName("Test Number Of Components "  + testMeasurement.GetAttributeName() );
    statusMeasurement.SetValue( testNumberOfComponents );
    std::cout << statusMeasurement << std::endl;
    }

  // todo Check the number of components and the type to determing the
  // best way to generate a 2D image of the error.



  //////////////////////////////////////////////////////
  // Now generate the difference between the two images
  typedef itk::tf::DifferenceImageFilter<ImageType, ImageType> DiffType;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput( baselineReader->GetOutput() );
  diff->SetTestInput( testReader->GetOutput() );
  diff->SetMagnitudeThreshold( intensityTolerance );
  diff->SetToleranceRadius( radiusTolerance );
  diff->UpdateLargestPossibleRegion();

  unsigned long status = 0;
  status = diff->GetNumberOfPixelsWithDifferences();
  
  testutil::DoubleNumeric statusMeasurement;
  statusMeasurement.SetValue( static_cast<double>(status) );
  statusMeasurement.SetAttributeName("ImageError "  + testMeasurement.GetAttributeName() );
  std::cout << statusMeasurement << std::endl;

  fileutil::PathName diffName = fileutil::PathName(ITK_REGRESSION_OUTPUT).Append(testImageFilename.Tail().GetPathName()+".diff.png");
  ImageThing<ImageType>( diff->GetOutput(), 
                         diffName, 
                         testMeasurement.GetAttributeName());
    
  fileutil::PathName baselineName = fileutil::PathName(ITK_REGRESSION_OUTPUT).Append(testImageFilename.Tail().GetPathName()+".base.png");  
  ImageThing<ImageType>( baselineReader->GetOutput(), 
                         baselineName, 
                         testMeasurement.GetAttributeName());
  
  fileutil::PathName testName = fileutil::PathName(ITK_REGRESSION_OUTPUT).Append(testImageFilename.Tail().GetPathName()+".test.png"); 
  ImageThing<ImageType>( testReader->GetOutput(), 
                         testName, 
                         testMeasurement.GetAttributeName());
  
  return (status > numberOfPixelsTolerance) ? 1 : 0;
}


  

}
