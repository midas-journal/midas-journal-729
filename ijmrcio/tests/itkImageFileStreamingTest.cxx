/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageFileReaderStreamingTest.cxx,v $
  Language:  C++
  Date:      $Date: 2009-02-11 22:30:23 $
  Version:   $Revision: 1.2 $

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
#include "itkExtractImageFilter.h"
#include "itkDifferenceImageFilter.h"
#include "itkPipelineMonitorImageFilter.h"
#include "itkStreamingImageFilter.h"
#include "itkLocalFactory.h"



#include "itkLocalFactory.h"
#include "itktfRegression.h"
#include "itkLocalRegressionSupport.h"


namespace { // local namespace



}

// This tests trys streaming combinations for the image file
// reader. Specifically, it tries the combinations of the output
// requesting streams, and the reader being forced not to stream.
class ImageFileStreamingTest:
  public itk::Regression 
{
protected:

  typedef unsigned char            PixelType;
  typedef itk::Image<PixelType,3>  ImageType;
  typedef ImageType::ConstPointer  ImageConstPointer;
  

protected:


  void itkImageFileWriterStreamingTest1( fileutil::PathName inputFileName, 
                                         fileutil::PathName outputFileName, 
                                         bool existingFile, 
                                         bool forceNoStreamingInput) 
  {      
    unsigned int numberOfDataPieces = 4;
    
    std::cout << "Running test with: " << existingFile << " " << forceNoStreamingInput << std::endl;

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
    
  }

  
// This test is designed to improve coverage and test boundary cases 
  int itkImageFileWriterStreamingTest2( std::string inputFileName,
                                        std::string outputFileName )
  {    
    typedef itk::ImageFileReader<ImageType>      ReaderType;
    typedef itk::ImageFileWriter<ImageType>      WriterType;

    // We remove the output file
    itksys::SystemTools::RemoveFile( outputFileName.c_str() ); 
    
    unsigned int numberOfDataPieces = 4;
  
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFileName );
    reader->SetUseStreaming( true );
    
    typedef itk::Local::PipelineMonitorImageFilter<ImageType> MonitorFilter;
    MonitorFilter::Pointer monitor = MonitorFilter::New();
    monitor->SetInput(reader->GetOutput());
  
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outputFileName );
    writer->SetInput(monitor->GetOutput());
    writer->SetNumberOfStreamDivisions(numberOfDataPieces);
    
    // for coverage
    if (std::string(outputFileName) != writer->GetFileName()) 
      return EXIT_FAILURE;
    
    // for coverage
    if (numberOfDataPieces != writer->GetNumberOfStreamDivisions()) 
      return EXIT_FAILURE;

    // write the whole image in 4 pieces
    std::cout << "=== Updating ==" << std::endl;
    writer->Update();
    
    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( 4 ), "Monitor::VerifyAllInputCanStream" );
    this->MeasurementNumericInteger( CompareImage<ImageType>( inputFileName, outputFileName ), "Number Of Pixels Different" );
    

    reader->Modified();
    // get the size of the image
    reader->GenerateOutputInformation();
    ImageType::RegionType largestRegion;
    largestRegion = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
    itk::ImageIORegion  ioregion(3);


    ////////////////////////////////////////////////
    // test 1x1 size
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]+largestRegion.GetSize()[0]/2 );
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]+largestRegion.GetSize()[1]/2 );
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]+largestRegion.GetSize()[2]/2 );
    ioregion.SetSize(0, 1);
    ioregion.SetSize(1, 1);
    ioregion.SetSize(2, 1);

    writer->SetIORegion(ioregion);
  
    std::cout << "=== Updating 1x1x1 IORegion ==" << std::endl;
    writer->Update();

  
    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( 1 ), "Monitor::VerifyAllInputCanStream" );
    this->MeasurementNumericInteger( CompareImage<ImageType>( inputFileName, outputFileName ), "Number Of Pixels Different" );


    reader->Modified();
    ////////////////////////////////////////////////
    // test 2x2 with odd offset
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]+largestRegion.GetSize()[0]/2 + 1);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]+largestRegion.GetSize()[1]/2 + 1);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]+largestRegion.GetSize()[2]/2 + 1);
    ioregion.SetSize(0, 2);
    ioregion.SetSize(1, 2);
    ioregion.SetSize(2, 2);

    writer->SetIORegion(ioregion);
  
    std::cout << "=== Updating 2x2x2 IORegion with odd offset ==" << std::endl;
    writer->Update();
  
    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( -1 ), "Monitor::VerifyAllInputCanStream" );
    this->MeasurementNumericInteger( CompareImage<ImageType>( inputFileName, outputFileName ), "Number Of Pixels Different" );


    reader->Modified();
    ////////////////////////////////////////////////
    // test long skiny 
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]);
    ioregion.SetSize(0, 1);
    ioregion.SetSize(1, 1);
    ioregion.SetSize(2, largestRegion.GetSize()[2]);

    writer->SetIORegion(ioregion);
  
    std::cout << "=== Updating 1x1xlong IORegion ==" << std::endl;
    writer->Update();

    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( -1 ), "Monitor::VerifyAllInputCanStream" );
    this->MeasurementNumericInteger( CompareImage<ImageType>( inputFileName, outputFileName ), "Number Of Pixels Different" );


    reader->Modified();
    ////////////////////////////////////////////////
    // test long skiny 
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]);
    ioregion.SetSize(0, 1);
    ioregion.SetSize(1, largestRegion.GetSize()[1]);
    ioregion.SetSize(2, 1);

    writer->SetIORegion(ioregion);
  
    std::cout << "=== Updating 1xlongx1 IORegion ==" << std::endl;
    writer->Update();

    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( -1 ), "Monitor::VerifyAllInputCanStream" );
    this->MeasurementNumericInteger( CompareImage<ImageType>( inputFileName, outputFileName ), "Number Of Pixels Different" );

    reader->Modified();
    ////////////////////////////////////////////////
    // test full region
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]);
    ioregion.SetSize(0, largestRegion.GetSize()[0]);
    ioregion.SetSize(1, largestRegion.GetSize()[1]);
    ioregion.SetSize(2, largestRegion.GetSize()[2]);

    writer->SetIORegion(ioregion);
  
    std::cout << "=== Updating Full IORegion ==" << std::endl;
    writer->Update();

    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( 4 ), "Monitor::VerifyAllInputCanStream" );
    this->MeasurementNumericInteger( CompareImage<ImageType>( inputFileName, outputFileName ), "Number Of Pixels Different" );

    reader->Modified();
    bool thrownException = false;
    ////////////////////////////////////////////////
    // test out of bounds region
    ioregion.SetIndex(0, largestRegion.GetIndex()[0] - 1);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]);
    ioregion.SetSize(0, largestRegion.GetSize()[0]);
    ioregion.SetSize(1, largestRegion.GetSize()[1]);
    ioregion.SetSize(2, largestRegion.GetSize()[2]);

    writer->SetIORegion(ioregion);
  
    try
      {    
      std::cout << "=== Updating out of bounds IORegion ==" << std::endl;
      writer->Update();
      }
    catch( itk::ExceptionObject & err )
      {    
      std::cout << "Caught expected exception" << std::endl;
      std::cout << err << std::endl;
      thrownException = true;
      }
  
    if (!thrownException)
      return EXIT_FAILURE;


    reader->Modified();
    thrownException = false;
    ////////////////////////////////////////////////
    // test out of bounds region
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]+largestRegion.GetSize()[0]/2 + 1);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]+largestRegion.GetSize()[1]/2 + 1);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]+largestRegion.GetSize()[2]/2 + 1);
    ioregion.SetSize(0, largestRegion.GetSize()[0]);
    ioregion.SetSize(1, largestRegion.GetSize()[1]);
    ioregion.SetSize(2, largestRegion.GetSize()[2]+1);

    writer->SetIORegion(ioregion);
  
    try
      {
      std::cout << "=== Updating out of bounds IORegion ==" << std::endl;
      writer->Update();
      }
    catch( itk::ExceptionObject & err )
      {    
      std::cout << "Caught expected exception" << std::endl;
      std::cout << err << std::endl;
      thrownException = true;
      }
  
    if (!thrownException)
      return EXIT_FAILURE;


  
    reader->Modified();
    thrownException = false;
    ////////////////////////////////////////////////
    // test when regions aren't matching
    ImageType::RegionType halfLargestRegion;
    halfLargestRegion.SetIndex(largestRegion.GetIndex());
    halfLargestRegion.SetSize(0, largestRegion.GetSize()[0]/2);
    halfLargestRegion.SetSize(1, largestRegion.GetSize()[1]/2);
    halfLargestRegion.SetSize(2, largestRegion.GetSize()[2]/2);
  
    monitor->GetOutput()->SetRequestedRegion(halfLargestRegion);
  
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]);
    ioregion.SetSize(0, largestRegion.GetSize()[0]/3);
    ioregion.SetSize(1, largestRegion.GetSize()[1]/3);
    ioregion.SetSize(2, largestRegion.GetSize()[2]/3);

    writer->SetIORegion(ioregion);

    std::cout << "=== Preparing mismatched IORegion ==" << std::endl <<std::flush;
    monitor->Update();
    monitor->VerifyAllInputCanStream(1);
    std::cout << "=== Updating mismatched IORegion ==" << std::endl;    
    writer->Update();
    
    this->MeasurementNumericBoolean( monitor->VerifyAllNoUpdate(), "Monitor::VerifyAllNoUpdate" );
    this->MeasurementNumericInteger( CompareImage<ImageType>( inputFileName, outputFileName ), "Number Of Pixels Different" );
    
    return EXIT_SUCCESS;
}



  void itkImageFileReaderStreamingTest2( const std::string filename )
  {
    
    typedef itk::ImageFileReader<ImageType>      ReaderType;
    
    ReaderType::Pointer baselineReader = ReaderType::New();
    baselineReader->SetFileName( filename );
    baselineReader->UpdateLargestPossibleRegion();

    ImageType::ConstPointer baselineImage = baselineReader->GetOutput();

    ReaderType::Pointer streamingReader = ReaderType::New();
    streamingReader->SetFileName( filename );
    streamingReader->UseStreamingOn();

    
    typedef itk::Local::PipelineMonitorImageFilter<ImageType> MonitorFilter;
    MonitorFilter::Pointer monitor = MonitorFilter::New();
    monitor->SetInput( streamingReader->GetOutput());
    
    
    typedef itk::ExtractImageFilter<ImageType, ImageType> ExtractImageFilter;
    ExtractImageFilter::Pointer extractor = ExtractImageFilter::New();
    extractor->SetInput( monitor->GetOutput() );
    
    
    const ImageType::RegionType largestRegion = baselineReader->GetOutput()->GetLargestPossibleRegion();
    ImageType::RegionType ioregion = baselineReader->GetOutput()->GetLargestPossibleRegion();
    
    streamingReader->Modified();

    ////////////////////////////////////////////////
    // test whole Image
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]);
    ioregion.SetSize(0, 1);
    ioregion.SetSize(1, largestRegion.GetSize()[1]);
    ioregion.SetSize(2, 1);
    
    extractor->SetExtractionRegion( ioregion );
    std::cout << "=== Updating full IORegion ==" << std::endl;
    extractor->Update();
    

    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( 1 ), "Monitor::VerifyAllInputCanStream" );
    
    this->MeasurementNumericInteger( this->CompareImage<ImageType>( extractor->GetOutput(), 
                                                                       baselineImage,
                                                                       ioregion ), 
                                     "Number Of Pixels Different" );

    
    ////////////////////////////////////////////////
    // test fullx1x1
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]+largestRegion.GetSize()[1]/2 + 1);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]+largestRegion.GetSize()[2]/2 + 1);
    ioregion.SetSize(0, largestRegion.GetSize()[0]);
    ioregion.SetSize(1, 1);
    ioregion.SetSize(2, 1);
    
    extractor->SetExtractionRegion( ioregion );
    std::cout << "=== Updating fullx1x1 IORegion ==" << std::endl;
    extractor->UpdateLargestPossibleRegion();
    
    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( 1 ), "Monitor::VerifyAllInputCanStream" );
    this->MeasurementNumericBoolean( this->CompareImage<ImageType>( extractor->GetOutput(), 
                                                                       baselineImage,
                                                            ioregion ), 
                                     "Number Of Pixels Different" );

    ////////////////////////////////////////////////
    // test 1xfullx1
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]+largestRegion.GetSize()[0]/2 + 1);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]+largestRegion.GetSize()[2]/2 + 1);
    ioregion.SetSize(0, 1);
    ioregion.SetSize(1, largestRegion.GetSize()[1]);
    ioregion.SetSize(2, 1);
    
    extractor->SetExtractionRegion( ioregion );
    std::cout << "=== Updating 1xfullx1 IORegion ==" << std::endl;
    extractor->UpdateLargestPossibleRegion();
    

    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( 1 ), "Monitor::VerifyAllInputCanStream" );
    this->MeasurementNumericBoolean( this->CompareImage<ImageType>( extractor->GetOutput(), 
                                                                       baselineImage,
                                                                       ioregion ), 
                                     "Number Of Pixels Different" );



    ////////////////////////////////////////////////
    // test 1x1xfull
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]+largestRegion.GetSize()[0]/2 + 1);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]+largestRegion.GetSize()[1]/2 + 1);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]);
    ioregion.SetSize(0, 1);
    ioregion.SetSize(1, 1);
    ioregion.SetSize(2, largestRegion.GetSize()[2]);
    
    extractor->SetExtractionRegion( ioregion ); 
    std::cout << "=== Updating 1x1xfull IORegion ==" << std::endl;
    extractor->UpdateLargestPossibleRegion();

    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( 1 ), "Monitor::VerifyAllInputCanStream" );

    this->MeasurementNumericBoolean( this->CompareImage<ImageType>( extractor->GetOutput(), 
                                                                       baselineImage,
                                                                       ioregion ),
                                     "Number Of Pixels Different" );

    ////////////////////////////////////////////////
    // test fullx3x3
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]+largestRegion.GetSize()[1]/2 + 1);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]+largestRegion.GetSize()[2]/2 + 1);
    ioregion.SetSize(0, largestRegion.GetSize()[0]);
    ioregion.SetSize(1, 3);
    ioregion.SetSize(2, 3);
  
    extractor->SetExtractionRegion( ioregion );
    std::cout << "=== Updating fullx3x3 IORegion ==" << std::endl;
    extractor->UpdateLargestPossibleRegion();


    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( 1 ), "Monitor::VerifyAllInputCanStream" );
    this->MeasurementNumericBoolean( this->CompareImage<ImageType>( extractor->GetOutput(), 
                                                                       baselineImage,
                                                                       ioregion ), 
                                     "Number Of Pixels Different" );

  
    ////////////////////////////////////////////////
    // test 3xfullx3
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]+largestRegion.GetSize()[0]/2 + 1);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]+largestRegion.GetSize()[2]/2 + 1);
    ioregion.SetSize(0, 3);
    ioregion.SetSize(1, largestRegion.GetSize()[1]);
    ioregion.SetSize(2, 3);

    extractor->SetExtractionRegion( ioregion );
    std::cout << "=== Updating 3xfullx3 IORegion ==" << std::endl;
    extractor->UpdateLargestPossibleRegion();

    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( 1 ), "Monitor::VerifyAllInputCanStream" );
    this->MeasurementNumericBoolean( this->CompareImage<ImageType>( extractor->GetOutput(), 
                                                                       baselineImage,
                                                                       ioregion ),
                                     "Number Of Pixels Different" );


    ////////////////////////////////////////////////
    // test 3x3xfull
    ioregion.SetIndex(0, largestRegion.GetIndex()[0]+largestRegion.GetSize()[0]/2 + 1);
    ioregion.SetIndex(1, largestRegion.GetIndex()[1]+largestRegion.GetSize()[1]/2 + 1);
    ioregion.SetIndex(2, largestRegion.GetIndex()[2]);
    ioregion.SetSize(0, 3);
    ioregion.SetSize(1, 3);
    ioregion.SetSize(2, largestRegion.GetSize()[2]);

    extractor->SetExtractionRegion( ioregion ); 
    std::cout << "=== Updating 3x3xfull IORegion ==" << std::endl;
    extractor->UpdateLargestPossibleRegion();
  

    this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( 1 ), "Monitor::VerifyAllInputCanStream" );

    this->MeasurementNumericBoolean( this->CompareImage
<ImageType>( extractor->GetOutput(), 
                                                                       baselineImage,
                                                                       ioregion ), 
                                     "Number Of Pixels Different" );
 
  }


  void RunTest( std::string filename, bool forceNoStreamingInput )
  {
    
    const unsigned int numberOfDataPieces = 4;

    typedef unsigned char             PixelType;
    typedef itk::Image<PixelType,3>   ImageType;
   
    
    typedef itk::ImageFileReader<ImageType>         ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( filename );
    reader->SetUseStreaming( true );
    
    typedef itk::Local::PipelineMonitorImageFilter<ImageType> MonitorFilter;
    MonitorFilter::Pointer monitor = MonitorFilter::New();
    monitor->SetInput( reader->GetOutput() );


    typedef itk::StreamingImageFilter<ImageType, ImageType> StreamingFilter;
    StreamingFilter::Pointer streamer = StreamingFilter::New();
    streamer->SetInput( monitor->GetOutput() );
    streamer->SetNumberOfStreamDivisions( numberOfDataPieces );


    try
      {
      // execute pipeline
      if ( forceNoStreamingInput )
        {
        monitor->UpdateLargestPossibleRegion();
        }
      else
        {
        streamer->UpdateLargestPossibleRegion();
        }
      }
    catch( itk::ExceptionObject & e )
      {
      this->MeasurementTextString( e.GetLocation(), "Caught Exception" );
      std::cout << e.GetDescription() << std::endl;
      }

    // create measurements of how we executed
    if ( forceNoStreamingInput )
      {
      this->MeasurementNumericBoolean( monitor->VerifyAllInputCanNotStream(), "Monitor::VerifyAllInputCanNotStream" );
      }
    else
      {
      this->MeasurementNumericBoolean( monitor->VerifyAllInputCanStream( numberOfDataPieces ), "Monitor::VerifyAllInputCanStream" );
      }

  }

  

  
  int Test(int argc, char* argv[])
  {
    if( argc < 2 )
      { 
      std::cerr << "Usage: " << argv[0] << " InputFileName" << std::endl;
      return EXIT_FAILURE;
      }

    std::string fileExtension = fileutil::PathName( argv[1] ).Extension();

    
    fileutil::PathName inputFileName = argv[1];

    RunTest( inputFileName.GetPathName(), true );
    RunTest( inputFileName.GetPathName(), false );

    itkImageFileReaderStreamingTest2( inputFileName.GetPathName() );

    itkImageFileWriterStreamingTest2( inputFileName.GetPathName(), this->GenerateFileName( fileExtension, itk::ITK_LOCAL_REGRESSION_OUTPUT_DIR  ) );

    
    itkImageFileWriterStreamingTest1( inputFileName, this->GenerateFileName( fileExtension, itk::ITK_LOCAL_REGRESSION_OUTPUT_DIR ), false, false );
    itkImageFileWriterStreamingTest1( inputFileName, this->GenerateFileName( fileExtension, itk::ITK_LOCAL_REGRESSION_OUTPUT_DIR  ), true, false );
    itkImageFileWriterStreamingTest1( inputFileName, this->GenerateFileName( fileExtension, itk::ITK_LOCAL_REGRESSION_OUTPUT_DIR  ), false, true );
    itkImageFileWriterStreamingTest1( inputFileName, this->GenerateFileName( fileExtension, itk::ITK_LOCAL_REGRESSION_OUTPUT_DIR  ), true, true );

    return EXIT_SUCCESS;
  }

};





int itkImageFileStreamingTest(int argc, char* argv[])
{
  
  itk::Local::LocalFactory::RegisterOneFactory();

  ImageFileStreamingTest test;

  test.PrependInFileSearchPath( fileutil::PathName( itk::ITK_LOCAL_REGRESSION_DATA_BASELINE ) );

  return test.Main(argc, argv);
}
