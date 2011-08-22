#include "itkImageFileWriter.h"
#include "itkRGBPixel.h"
#include "itkImageRegionIterator.h"
#include "itkNumericTraits.h"

#include "itkLocalFactory.h"
#include "itkMRCHeaderObject.h"
#include "itktfRegression.h"

#include "itkLocalRegressionSupport.h"



class MRCImageFileIOTest:
    public itk::Regression
{    
protected:
  static unsigned int callNumber;

  template <typename TImageType>
  void WriteMRCImage( const std::string &filePrefix, fileutil::PathName outputPath )
  {
    try 
      {
      ++callNumber;

      
      typedef TImageType                    ImageType;
      typedef typename ImageType::PixelType PixelType;

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
            i.Set( itk::NumericTraits<PixelType>::min( PixelType()) );
            break;
          case 3:
            i.Set( itk::NumericTraits<PixelType>::max( PixelType()) );
          }
        ++cnt;
        ++i;
        }
    
      typedef itk::ImageFileWriter<ImageType> ImageFileWriterType;
      typename ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
      writer->SetInput( image );

      std::ostringstream nameWithIndex;
      nameWithIndex << filePrefix << "_" << callNumber << ".mrc";

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

    
    this->MeasurementNumericInteger( sizeof(itk::Local::MRCHeaderObject::Header), "Size of MRCHeader" );

    this->MeasurementNumericInteger( sizeof(itk::Local::MRCHeaderObject::FeiExtendedHeader), "Size of MRCHeaderExtendedHeader" );

    // test all usable pixeltypes
    this->WriteMRCImage< itk::Image<unsigned char, 3> >( filePrefix, outpuPath );
    this->WriteMRCImage< itk::Image<short, 3> >( filePrefix, outpuPath );
    this->WriteMRCImage< itk::Image<float, 3> >( filePrefix, outpuPath );
    this->WriteMRCImage< itk::Image<unsigned short, 3> >( filePrefix, outpuPath );
    this->WriteMRCImage< itk::Image<itk::RGBPixel<unsigned char>, 3 > >( filePrefix, outpuPath );
    this->WriteMRCImage< itk::Image<std::complex<float>, 3 > >( filePrefix, outpuPath );
    
    // test additional usable dimensions
    this->WriteMRCImage< itk::Image<unsigned char, 1> >( filePrefix, outpuPath );
    this->WriteMRCImage< itk::Image<unsigned char, 2> >( filePrefix, outpuPath );

    // expect exceptions with the following
    this->WriteMRCImage< itk::Image<double> >( filePrefix, outpuPath );
    this->WriteMRCImage< itk::Image<int> >( filePrefix, outpuPath );
    this->WriteMRCImage< itk::Image<unsigned long> >( filePrefix, outpuPath );

    // test unusable dimensions
    this->WriteMRCImage< itk::Image<unsigned char, 4> >( filePrefix, outpuPath );
    this->WriteMRCImage< itk::Image<unsigned char, 5> >( filePrefix, outpuPath );

    return EXIT_SUCCESS;
  }
};

unsigned int MRCImageFileIOTest::callNumber = 0;




int itkMRCImageFileIOTest(int argc, char* argv[])
{

  itk::Local::LocalFactory::RegisterOneFactory();

  MRCImageFileIOTest test;

  test.PrependInFileSearchPath( fileutil::PathName( itk::ITK_LOCAL_REGRESSION_DATA_BASELINE ) );
  
  return test.Main(argc, argv);
}
