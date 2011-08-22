/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageFileWriterStreamingTest1.cxx,v $
  Language:  C++
  Date:      $Date: 2009-02-18 16:13:23 $
  Version:   $Revision: 1.3 $

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
#include "itkPipelineMonitorImageFilter.h"


#include "itkLocalFactory.h"
#include "itktfRegression.h"
#include "itkLocalRegressionSupport.h"


// This test it to try combinations of if the output file alreadys
// exists and when the input filter does not stream
class ImageFileWriterStreamingTest1:
    public itk::Regression
{    
protected:  
  unsigned int m_CallNumber;

  void RunTest1(  const std::string &filePrefix,  
                  fileutil::PathName inputFileName, 
                  fileutil::PathName outputPath, 
                  bool existingFile, 
                  bool forceNoStreamingInput) 
  {      
    unsigned int numberOfDataPieces = 4;

    std::cout << "Running test with: "
              << existingFile << " " << forceNoStreamingInput << std::endl;

    typedef unsigned char            PixelType;
    typedef itk::Image<PixelType,3>   ImageType;
    
    typedef itk::ImageFileReader<ImageType>         ReaderType;
    typedef itk::ImageFileWriter< ImageType >  WriterType;
    
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( inputFileName.GetPathName() );
    reader->SetUseStreaming( true );
    
    typedef itk::Local::PipelineMonitorImageFilter<ImageType> MonitorFilter;
    MonitorFilter::Pointer monitor = MonitorFilter::New();
    monitor->SetInput(reader->GetOutput());

    if ( forceNoStreamingInput )
      {
      reader->UseStreamingOff();
      }
    
    std::ostringstream nameWithIndex;
    nameWithIndex <<  filePrefix  << "_" << m_CallNumber << "." << fileExtension;
    fileutil::PathName outputFileName = outputPath;
    outputFileName.Append( nameWithIndex.str() );


    // Setup the writer
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outputFileName.GetPathName()  );
    writer->SetInput(monitor->GetOutput());
    writer->SetNumberOfStreamDivisions(numberOfDataPieces);
    
    try 
      {
      writer->Update();
      } 
    catch (itk::ExceptionObject &e) 
      {
      this->MeasurementTextString( e.GetLocation(), "Caught Exception" );
      std::cout << e.GetDescription() << std::endl;
      }

  
    this->MeasurementInsightFileImage( outputFileName.GetPathName(), "OutputImage" );
  
    if ( forceNoStreamingInput )
      {
      this->MeasurementNumericBoolean( monitor->VerifyAllInputCanNotStream(), "Monitor::VerifyAllInputCanNotStream" );
      }
    else
      {
      this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( numberOfDataPieces ), "Monitor::VerifyAllInputCanStream" );
      }
    
    ++m_CallNumber;
  }
  // move me
  std::string fileExtension;
  int Test(int argc, char* argv[])
  {
    if( argc < 3 )
      { 
      std::cerr << "Usage: " << argv[0] << " fileExtension inputFileName outputPath" << std::endl;
      return EXIT_FAILURE;
      }

    m_CallNumber = 1;
    
    fileExtension = argv[1];
    fileutil::PathName inputFileName = argv[2];
    fileutil::PathName outputPath = argv[3];
    std::string filePrefix = argv[0];
      
    RunTest1( filePrefix, inputFileName, outputPath, false, false );
    RunTest1( filePrefix, inputFileName, outputPath, true, false );
    RunTest1( filePrefix, inputFileName, outputPath, false, true );
    RunTest1( filePrefix, inputFileName, outputPath, true, true );

    return EXIT_SUCCESS;
  }
};

int itkImageFileWriterStreamingTest1(int argc, char* argv[])
{
  
  itk::Local::LocalFactory::RegisterOneFactory();

  ImageFileWriterStreamingTest1 test;

  test.PrependInFileSearchPath( fileutil::PathName( itk::ITK_LOCAL_REGRESSION_DATA_BASELINE ) );

  return test.Main(argc, argv);
}
