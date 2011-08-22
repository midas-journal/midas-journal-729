#include "itkImageFileReader.h"

#include "itkLocalFactory.h"
#include "itktfRegression.h"

class ImageFileReaderInfoTest:
    public itk::Regression
{    
protected:
  virtual int Test(int argc, char* argv[] )
  {
  
    if( argc < 2 )
      {
      std::cerr << "Usage: " << argv[0] << " inputFile" << std::endl;
      return EXIT_FAILURE;
      }
    
  

    typedef unsigned char             PixelType;
    typedef itk::Image<PixelType,3>   ImageType;

    typedef itk::ImageFileReader<ImageType>         ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( argv[1] );

    reader->UpdateOutputInformation();

    ImageType::Pointer image = reader->GetOutput();
    ImageType::RegionType region = image->GetLargestPossibleRegion();
    itk::ImageIOBase::Pointer imageIO = reader->GetImageIO();
       
    this->MeasurementInsightSize( region.GetSize(), "ImageSize" );
    this->MeasurementInsightVector( image->GetSpacing(), "ImageSpacing" );
    this->MeasurementInsightPoint( image->GetOrigin(), "ImageOrigin" );
    this->MeasurementInsightMatrix( image->GetDirection(), "ImageDirection" );
    this->MeasurementTextString( imageIO->GetFileTypeAsString( imageIO->GetFileType()), "FileTypeAsString" );
    this->MeasurementTextString( imageIO->GetByteOrderAsString( imageIO->GetByteOrder() ), "ByteOrderAsString" );
    this->MeasurementTextString( imageIO->GetPixelTypeAsString( imageIO->GetPixelType() ), "PixelTypeAsString" );
    this->MeasurementTextString( imageIO->GetComponentTypeAsString( imageIO->GetComponentType() ), "ComponentTypeAsString" );
    this->MeasurementNumericInteger( imageIO->GetNumberOfComponents( ), "NumberOfComponents" );
    

  
    return EXIT_SUCCESS;
  }
};



int itkImageFileReaderInfoTest(int argc, char* argv[])
{
  itk::Local::LocalFactory::RegisterOneFactory();
    
  ImageFileReaderInfoTest test;
  return test.Main(argc, argv);
}
