/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageFileWriterStreamingPastingCompressingTest1.cxx,v $
  Language:  C++
  Date:      $Date: 2009-03-27 13:45:40 $
  Version:   $Revision: 1.9 $

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

#include <stdio.h>
#include <fstream>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkDifferenceImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkPipelineMonitorImageFilter.h"
#include "itkLocalFactory.h"


typedef unsigned char            PixelType;
typedef itk::Image<PixelType,3>  ImageType;
typedef ImageType::Pointer       ImagePointer;

namespace {


bool SameImage(ImagePointer testImage, ImagePointer baselineImage)
{
  PixelType intensityTolerance = 5;  // need this for compression
  int radiusTolerance = 0;
  unsigned long numberOfPixelTolerance = 0;
    
  typedef itk::DifferenceImageFilter<ImageType,ImageType> DiffType;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput(baselineImage);
  diff->SetTestInput(testImage);
  diff->SetDifferenceThreshold( intensityTolerance );
  diff->SetToleranceRadius( radiusTolerance );
  diff->UpdateLargestPossibleRegion();

  unsigned long status = diff->GetNumberOfPixelsWithDifferences();

  if (status > numberOfPixelTolerance)
    {
    return false;
    }

  return true;
}

bool SameImage(std::string testImageFileName, ImagePointer baselineImage)
{
  PixelType intensityTolerance = 5; // need this for compression
  int radiusTolerance = 0;
  unsigned long numberOfPixelTolerance = 0;

  
  typedef itk::ImageFileReader<ImageType>    ReaderType;
  ReaderType::Pointer readerTestImage = ReaderType::New();
  readerTestImage->SetFileName( testImageFileName );
    
  typedef itk::DifferenceImageFilter<ImageType,ImageType> DiffType;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput(baselineImage);
  diff->SetTestInput(readerTestImage->GetOutput());
  diff->SetDifferenceThreshold( intensityTolerance );
  diff->SetToleranceRadius( radiusTolerance );
  diff->UpdateLargestPossibleRegion();

  unsigned long status = diff->GetNumberOfPixelsWithDifferences();

  if (status > numberOfPixelTolerance)
    {
    std::cout << "NumberOfPixelsWithDifference: " << status << std::endl;
    return false;
    }

  return true;
}

bool ActualTest(std::string inputFileName, std::string outputFileNameBase, std::string outputFileNameExtension, bool streamWriting, bool pasteWriting, bool compressWriting, int expectException = -1) 
{
  
  std::cout << "Writing Combination: " << streamWriting << " " << pasteWriting << " " << compressWriting << std::endl;

  ::itk::OStringStream outputFileNameStream;
  outputFileNameStream << outputFileNameBase << streamWriting << pasteWriting << compressWriting << "." << outputFileNameExtension;
  std::string outputFileName = outputFileNameStream.str();

  
  std::cout << "Writing to File: " << outputFileName << std::endl;
  unsigned int m_NumberOfPieces = 10;
  
  // We remove the output file
  itksys::SystemTools::RemoveFile(outputFileName.c_str());
  
  typedef unsigned char            PixelType;
  typedef itk::Image<PixelType,3>   ImageType;

  typedef itk::ImageFileReader<ImageType>         ReaderType;
  typedef itk::ImageFileWriter< ImageType >  WriterType;

  

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFileName.c_str() );
  reader->SetUseStreaming( true );

  // read the region info
  reader->GenerateOutputInformation();

  ImageType::RegionType largestRegion;
  largestRegion = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  
  ImageType::IndexType pasteIndex;
  pasteIndex[0] = largestRegion.GetIndex()[0]+largestRegion.GetSize()[0]/3;
  pasteIndex[1] = largestRegion.GetIndex()[1]+largestRegion.GetSize()[1]/3;
  pasteIndex[2] = largestRegion.GetIndex()[2]+largestRegion.GetSize()[2]/3;
  ImageType::SizeType pasteSize;
  pasteSize[0] = largestRegion.GetSize()[0]/3;
  pasteSize[1] = largestRegion.GetSize()[1]/3;
  pasteSize[2] = 1;
  ImageType::RegionType pasteRegion(pasteIndex, pasteSize);

  
  typedef itk::Local::PipelineMonitorImageFilter<ImageType> MonitorFilter;
  MonitorFilter::Pointer monitor = MonitorFilter::New();
  monitor->SetInput(reader->GetOutput());
  

  
  // Setup the writer
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputFileName);
  writer->SetInput(monitor->GetOutput());

  // create a vaild region from the largest
  itk::ImageIORegion  ioregion(3);
  itk::ImageIORegionAdaptor<3>::Convert(pasteRegion, ioregion, largestRegion.GetIndex());

  if (streamWriting) 
    {
    writer->SetNumberOfStreamDivisions( m_NumberOfPieces );
    }
  
  if (pasteWriting) 
    {
    writer->SetIORegion(ioregion);
    }

  writer->SetUseCompression(compressWriting);

  try
    {
    
    writer->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    if (expectException == -1 || expectException == 1)  
      {      
      std::cout << "Expected ExceptionObject caught !" << std::endl;      
      std::cout << err << std::endl;
      std::cout << "TEST PASSED" << std::endl;
      return EXIT_SUCCESS;
      }
    else 
      {
      std::cout << "UnExpected ExceptionObject caught !" << std::endl;
      std::cout << err << std::endl;
      std::cout << "TEST FAILED" << std::endl;
      return EXIT_FAILURE;
      }
    }

  if ( expectException == 1 ) 
    {
    std::cout << "Did not get expected exception!" << std::endl;
    std::cout << "TEST FAILED" << std::endl;
    return EXIT_FAILURE;
    }

   
  // if we didn't have an exception then we should have produced the
  // correct image
  if (pasteWriting) 
    {
    typedef itk::ExtractImageFilter<ImageType, ImageType> ExtractImageFilterType;
    ExtractImageFilterType::Pointer extractBaselineImage = ExtractImageFilterType::New();
    extractBaselineImage->SetInput(reader->GetOutput());
    extractBaselineImage->SetExtractionRegion(pasteRegion);
    
    ReaderType::Pointer readerTestImage = ReaderType::New();
    readerTestImage->SetFileName( outputFileName );
    ExtractImageFilterType::Pointer extractTestImage = ExtractImageFilterType::New();  
    extractTestImage->SetInput(readerTestImage->GetOutput());
    extractTestImage->SetExtractionRegion(pasteRegion);
    
    if (!SameImage(extractTestImage->GetOutput(), extractBaselineImage->GetOutput())) 
      {
      std::cout << "Paste regions of images differ" << std::endl;
      std::cout << "TEST FAILED" << std::endl;
      return EXIT_FAILURE;
      }

    }
  else if (!SameImage(outputFileName, reader->GetOutput())) 
    {
    std::cout << "Images differ" << std::endl;
    std::cout << "TEST FAILED" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "TEST PASSED" << std::endl;
  return EXIT_SUCCESS;
}

}


int itkImageFileWriterStreamingPastingCompressingTest1(int argc, char* argv[])
{
  
  //  line added for my itkLocal ImageIOs
  itk::Local::LocalFactory::RegisterOneFactory();

  if( argc < 3 )
    {
    std::cerr << "Usage: " << argv[0] << " input outputBase outputExtension [expect exception (0|1)] ..." << std::endl;
    return EXIT_FAILURE;
    }

  int expectException[8];
  const int expectedExceptionOffset = 4;
  int i;
  for ( i = 0; i < 8; ++i) 
    {
    if (argc > i + expectedExceptionOffset) 
      {
      expectException[i] = 0;
      if (atoi(argv[i+expectedExceptionOffset]) == 1)         
        expectException[i] = 1;
      }
    else 
      expectException[i] = -1;
    }

  int retValue = EXIT_SUCCESS;
  i = 0;

  retValue = (retValue == EXIT_FAILURE) ? EXIT_FAILURE : ActualTest(argv[1], argv[2], argv[3], 0, 0, 0, expectException[i++]);
  retValue = (retValue == EXIT_FAILURE) ? EXIT_FAILURE : ActualTest(argv[1], argv[2], argv[3], 0, 0, 1, expectException[i++]);
  retValue = (retValue == EXIT_FAILURE) ? EXIT_FAILURE : ActualTest(argv[1], argv[2], argv[3], 0, 1, 0, expectException[i++]);
  retValue = (retValue == EXIT_FAILURE) ? EXIT_FAILURE : ActualTest(argv[1], argv[2], argv[3], 0, 1, 1, expectException[i++]);
  retValue = (retValue == EXIT_FAILURE) ? EXIT_FAILURE : ActualTest(argv[1], argv[2], argv[3], 1, 0, 0, expectException[i++]);
  retValue = (retValue == EXIT_FAILURE) ? EXIT_FAILURE : ActualTest(argv[1], argv[2], argv[3], 1, 0, 1, expectException[i++]);
  retValue = (retValue == EXIT_FAILURE) ? EXIT_FAILURE : ActualTest(argv[1], argv[2], argv[3], 1, 1, 0, expectException[i++]);
  retValue = (retValue == EXIT_FAILURE) ? EXIT_FAILURE : ActualTest(argv[1], argv[2], argv[3], 1, 1, 1, expectException[i++]);
  
  
  return retValue;
}
