#include "itktfRegression.h"


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include "itkRandomImageSource.h"
#include "itkAddImageFilter.h"

namespace itk 
{

class Test3 :
  public itk::Regression
{
protected:
  virtual int Test( int argc, char *argv[] )
  {

    
  if(argc < 3)
    {
    std::cerr << "Usage: " << argv[0] << " InputImage OutputImage\n";
    return EXIT_FAILURE;
    }

    typedef itk::Image<float,3> ImageType;
    
    fileutil::PathName inputFilename = argv[1];
    fileutil::PathName outputFilename = argv[2];

    // Now generate a real image
    typedef itk::ImageFileReader<ImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( inputFilename.GetPathName() );
    reader->UpdateOutputInformation();

    unsigned long size[3];
    size[0] = reader->GetOutput()->GetLargestPossibleRegion().GetSize(0);
    size[1] = reader->GetOutput()->GetLargestPossibleRegion().GetSize(1);
    size[2] = reader->GetOutput()->GetLargestPossibleRegion().GetSize(2);

    // we create a random image +/-0.5
    typedef itk::RandomImageSource<ImageType> RandomSourceType;
    RandomSourceType::Pointer randomSource = RandomSourceType::New();
    randomSource->SetSize( size );
    randomSource->SetMin(-.5);
    randomSource->SetMax(.5);

    
    typedef itk::AddImageFilter<ImageType,ImageType,ImageType> AddFilterType;
    AddFilterType::Pointer addFilter = AddFilterType::New();
    addFilter->SetInput1(reader->GetOutput());
    addFilter->SetInput2(randomSource->GetOutput());

    typedef itk::ImageFileWriter<ImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outputFilename.GetPathName() );
    writer->SetInput( addFilter->GetOutput() );
    writer->Update();


    // TestingFramework changes
    // The tolerance for the image comparison is first set
    // Then the image is specified as a measurement for regression
    this->SetImageInsightTolerance( 1, 0, 0 );
    this->MeasurementInsightFileImage( outputFilename.GetPathName(), "Tolerant Image Output" );

    return EXIT_SUCCESS;
  }
};

}

int main( int argc, char *argv[] )
{
  itk::Test3 test; 
  return test.Main( argc, argv );
}
