#include "itkImageFileWriter.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"
#include "itkImageRegionIterator.h"
#include "itkNumericTraits.h"

#include "itkLocalFactory.h"
#include "itktfRegression.h"

#include "itkLocalRegressionSupport.h"
#include "../itkVTKImageIO.h"


class VTKImageFileIOTest:
    public itk::Regression
{    
protected:
  static unsigned int callNumber;

  template <typename TPixelType, unsigned int Dimension>
  void WriteVTKImage( const std::string &filePrefix, fileutil::PathName outputPath, bool ascii )
  {
    try 
      {
      ++callNumber;

      typedef TPixelType                PixelType;
      typedef itk::Image<PixelType,Dimension>   ImageType;

      // allocate an 10x10x10 image    
      typename ImageType::Pointer image = ImageType::New();    
      typename ImageType::SizeType imageSize;
      imageSize.Fill(10);
      image->SetRegions( imageSize );
      image->Allocate();

      unsigned int cnt = 0;
      itk::ImageRegionIterator< ImageType > i( image, image->GetLargestPossibleRegion() );
      i.GoToBegin();
      while (! i.IsAtEnd() )
        {
        // fill the image switching between these pixels
        switch (cnt%4) 
          {
          case 0:
            i.Set( itk::NumericTraits<PixelType>::ZeroValue() );
            break;
          case 1:
            i.Set( itk::NumericTraits<PixelType>::OneValue() );
            break;
          case 2:
            i.Set( itk::NumericTraits<PixelType>::min( ) );
            break;
          case 3:
            i.Set( itk::NumericTraits<PixelType>::max( ) );
          }
        ++cnt;
        ++i;
        }

      itk::Local::VTKImageIO::Pointer vtkIO = itk::Local::VTKImageIO::New();
      vtkIO->SetFileTypeToASCII();
      if (ascii)
        {
        vtkIO->SetFileTypeToASCII();
        }
      else
        {
        vtkIO->SetFileTypeToBinary();
        }
    
      typedef itk::ImageFileWriter<ImageType> ImageFileWriterType;
      typename ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
      writer->SetImageIO( vtkIO );
      writer->SetInput( image );

      std::ostringstream nameWithIndex;
      nameWithIndex << filePrefix << "_" << callNumber << ".vtk";

      fileutil::PathName outputFileName = outputPath;
      outputFileName.Append( nameWithIndex.str() );
      writer->SetFileName( outputFileName.GetPathName() );
      writer->Update();  

    
      this->MeasurementInsightFileImage( outputFileName.GetPathName(), nameWithIndex.str() );
      } 
    catch (itk::ExceptionObject &e) 
      {
      this->MeasurementTextString( e.GetLocation(), "Caught Exception" );
      std::cout << e.GetDescription() << std::endl;
      }
      
  }
  

  virtual int Test(int argc, char* argv[] )
  {    
    
    if( argc < 2 )
      {
      std::cerr << "Usage: " << argv[0] << " outputPath" << std::endl;
      return EXIT_FAILURE;
      }
    
    fileutil::PathName outpuPath = argv[1];
    std::string filePrefix = argv[0];
    

    this->WriteVTKImage<unsigned char, 3>( filePrefix, outpuPath, true );
    this->WriteVTKImage<unsigned char, 3>( filePrefix, outpuPath, false );
    this->WriteVTKImage<char, 3>( filePrefix, outpuPath, true );
    this->WriteVTKImage<char, 3>( filePrefix, outpuPath, false );

    this->WriteVTKImage<unsigned short, 3>( filePrefix, outpuPath, true );
    this->WriteVTKImage<unsigned short, 3>( filePrefix, outpuPath, false );
    this->WriteVTKImage<short, 3>( filePrefix, outpuPath, true );
    this->WriteVTKImage<short, 3>( filePrefix, outpuPath, false );

    this->WriteVTKImage<unsigned int, 3>( filePrefix, outpuPath, true );
    this->WriteVTKImage<unsigned int, 3>( filePrefix, outpuPath, false );
    this->WriteVTKImage<int, 3>( filePrefix, outpuPath, true );
    this->WriteVTKImage<int, 3>( filePrefix, outpuPath, false );
    
    // long is not portable and dumb, so we skip

    
    this->WriteVTKImage< itk::RGBPixel<unsigned char>, 3>( filePrefix, outpuPath, true );
    this->WriteVTKImage< itk::RGBPixel<unsigned char>, 3>( filePrefix, outpuPath, false );
    this->WriteVTKImage< itk::RGBAPixel<unsigned char>, 3>( filePrefix, outpuPath, true );
    this->WriteVTKImage< itk::RGBAPixel<unsigned char>, 3>( filePrefix, outpuPath, false );

    
    this->WriteVTKImage< itk::Vector<int, 3>, 3 >( filePrefix, outpuPath, true );
    this->WriteVTKImage< itk::Vector<int, 3>, 3 >( filePrefix, outpuPath, false );
    this->WriteVTKImage< itk::Vector<double, 3>, 3 >( filePrefix, outpuPath, true );
    this->WriteVTKImage< itk::Vector<double, 3>, 3 >( filePrefix, outpuPath, false );

    this->WriteVTKImage<float, 3>( filePrefix, outpuPath, true );
    this->WriteVTKImage<float, 3>( filePrefix, outpuPath, false );
    this->WriteVTKImage<double, 3>( filePrefix, outpuPath, true );
    this->WriteVTKImage<double, 3>( filePrefix, outpuPath, false );


    

    return EXIT_SUCCESS;
  }
};

unsigned int VTKImageFileIOTest::callNumber = 0;




int itkVTKImageFileIOTest(int argc, char* argv[])
{
  itk::Local::LocalFactory::RegisterOneFactory();

  VTKImageFileIOTest test;

  test.PrependInFileSearchPath( fileutil::PathName( itk::ITK_LOCAL_REGRESSION_DATA_BASELINE ) );
  
  return test.Main(argc, argv);
}
