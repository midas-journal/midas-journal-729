/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageFileWriterPastingTest2.cxx,v $
  Language:  C++
  Date:      $Date: 2009-01-31 04:55:53 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include <fstream>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkDifferenceImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkPipelineMonitorImageFilter.h"

#include "itkLocalFactory.h"
#include "itktfRegression.h"
#include "itkLocalRegressionSupport.h"

#include "itkImageRegionIteratorWithIndex.h"


class ImageFileWritePastingTest2 :
  public itk::Regression 
{
public:
  
typedef unsigned char            PixelType;
typedef itk::Image<PixelType,3>  ImageType;
typedef ImageType::Pointer       ImagePointer;

protected:

  unsigned int m_CallNumber;

  void  RunTest1( const std::string &filePrefix,  
                  fileutil::PathName inputFileName, 
                  fileutil::PathName outputFileName)
  {    
    typedef itk::ImageFileReader<ImageType>         ReaderType;
    typedef itk::ImageFileWriter< ImageType >  WriterType;
    
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( inputFileName.GetPathName() );
    reader->SetUseStreaming( true );
  
    // We decide how we want to read the image and we split accordingly
    // The image is read slice by slice
    reader->GenerateOutputInformation();

    ImageType::RegionType largestRegion;
    largestRegion = reader->GetOutput()->GetLargestPossibleRegion();
  
    ImageType::IndexType pasteIndex;
    pasteIndex[0] = largestRegion.GetIndex()[0]+largestRegion.GetSize()[0]/3;
    pasteIndex[1] = largestRegion.GetIndex()[1]+largestRegion.GetSize()[1]/3;
    pasteIndex[2] = largestRegion.GetIndex()[2]+largestRegion.GetSize()[2]/3;
    ImageType::SizeType pasteSize;
    pasteSize[0] = largestRegion.GetSize()[0]/3;
    pasteSize[1] = largestRegion.GetSize()[1]/3;
    pasteSize[2] = largestRegion.GetSize()[2]/3;
    ImageType::RegionType pasteRegion(pasteIndex, pasteSize);
  
    typedef itk::Local::PipelineMonitorImageFilter<ImageType> MonitorFilter;
    MonitorFilter::Pointer monitor = MonitorFilter::New();
    monitor->SetInput(reader->GetOutput());


    // Setup the writer
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outputFileName.GetPathName() );
    writer->SetInput(monitor->GetOutput());
    
    // create a vaild region from the largest
    itk::ImageIORegion  ioregion(3);
    itk::ImageIORegion::IndexType index;
    
    index.push_back(pasteIndex[0]);
    index.push_back(pasteIndex[1]);
    index.push_back(pasteIndex[2]);
    ioregion.SetIndex(index);
    itk::ImageIORegion::SizeType size;
    size.push_back(pasteSize[0]);
    size.push_back(pasteSize[1]);
    size.push_back(pasteSize[2]);
    ioregion.SetSize(size);
    writer->SetIORegion(ioregion);
    
    try 
      {
      writer->Update();
      }
    catch (itk::ExceptionObject &e) 
      {
      this->MeasurementTextString( e.GetLocation(), "Caught Exception" );
      std::cout << e.GetDescription() << std::endl;
      return;
      }
  


    this->MeasurementNumericInteger( monitor->GetNumberOfUpdates(), "Monitor::GetNumerOfUpdates" );
    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream(1), "Monitor::VerifyAllInputCanStream" );
    
    
    // we need to compare just the subregions, so we can't use the
    // standard image measurement
    // compare subregion of input file and output file
    
    typedef itk::ExtractImageFilter<ImageType, ImageType> ExtractImageFilterType;
    ExtractImageFilterType::Pointer extractBaselineImage = ExtractImageFilterType::New();
    extractBaselineImage->SetInput(reader->GetOutput());
    extractBaselineImage->SetExtractionRegion(pasteRegion);
    
    ReaderType::Pointer readerTestImage = ReaderType::New();
    readerTestImage->SetFileName( outputFileName.GetPathName() );
    ExtractImageFilterType::Pointer extractTestImage = ExtractImageFilterType::New();  
    extractTestImage->SetInput(readerTestImage->GetOutput());
    extractTestImage->SetExtractionRegion(pasteRegion);
    

    PixelType intensityTolerance = 0;
    int radiusTolerance = 0;
    unsigned long numberOfPixelTolerance = 0;
    
    typedef itk::DifferenceImageFilter<ImageType,ImageType> DiffType;
    DiffType::Pointer diff = DiffType::New();
    diff->SetValidInput(extractBaselineImage->GetOutput());
    diff->SetTestInput(extractTestImage->GetOutput());
    diff->SetDifferenceThreshold( intensityTolerance );
    diff->SetToleranceRadius( radiusTolerance );
    diff->UpdateLargestPossibleRegion();

    this->MeasurementNumericInteger(  diff->GetNumberOfPixelsWithDifferences(), "Number Of Pixels Different" );

   
    return;
  }
  
  int Test(int argc, char* argv[])
  {
    if( argc < 4 )
      { 
      std::cerr << "Usage: " << argv[0] << " fileExtension outputPath inputFileName copyFileName" << std::endl;
      return EXIT_FAILURE;
      }
    
    m_CallNumber = 0;

    std::string fileExtension = argv[1];
    const fileutil::PathName outputPath = argv[2];
    const fileutil::PathName inputFileName = argv[3];
    fileutil::PathName copyFileName;
    fileutil::PathName outputFileName;
    std::string filePrefix = argv[0]; // assume the test gives us a
                                      // unique name for the output`
                                      // file

    

    
    std::ostringstream nameWithCallNumber;
    nameWithCallNumber <<  filePrefix  << "_" << m_CallNumber++ << "." << fileExtension;
    outputFileName = outputPath;
    outputFileName.Append( nameWithCallNumber.str() );

    itksys::SystemTools::RemoveFile( outputFileName.GetPathName().c_str() ); 
    RunTest1( filePrefix, inputFileName, outputFileName );
    
    

    for (int i = 4; i < argc; ++i )
      {
      copyFileName = argv[i];
      nameWithCallNumber.str( "" );
      nameWithCallNumber <<  filePrefix  << "_" << m_CallNumber++ << "." << fileExtension;
      outputFileName = outputPath;
      outputFileName.Append( nameWithCallNumber.str() );
      

      this->MeasurementTextString( "Pasting to copy of " + copyFileName.Tail().GetPathName() + " as " + outputFileName.Tail().GetPathName(), "Testing" );
      itksys::SystemTools::CopyAFile( copyFileName.GetPathName().c_str(), outputFileName.GetPathName().c_str() );
      RunTest1( filePrefix, inputFileName, outputFileName );
      }
    
    return EXIT_SUCCESS;
  }

};




int  itkImageFileWriterPastingTest2(int argc, char* argv[])
{


  itk::Local::LocalFactory::RegisterOneFactory();

  ImageFileWritePastingTest2 test;

  test.PrependInFileSearchPath( fileutil::PathName( itk::ITK_LOCAL_REGRESSION_DATA_BASELINE ) );

  return test.Main(argc, argv);
}

