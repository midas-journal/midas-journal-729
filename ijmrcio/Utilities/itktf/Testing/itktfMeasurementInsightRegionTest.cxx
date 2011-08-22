#include "itktfRegression.h"


#include "itkImageFileReader.h"

namespace itk 
{

class MeasurementInsightRegionTest :
  public itk::Regression
{
protected:
  virtual int Test( int argc, char *argv[] )
  {

    
  if(argc < 2)
    {
    std::cerr << "Usage: " << argv[0] << " InputImage\n";
    return EXIT_FAILURE;
    }

    typedef itk::Image<float,3> ImageType;
    
    fileutil::PathName inputFilename = argv[1];

    typedef itk::ImageFileReader<ImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( inputFilename.GetPathName() );
    reader->UpdateOutputInformation();
    
    this->MeasurementInsightRegion( reader->GetOutput()->GetLargestPossibleRegion(), "Read Region" );

    return EXIT_SUCCESS;
  }
};

}

int main( int argc, char *argv[] )
{
  itk::MeasurementInsightRegionTest test; 
  return test.Main( argc, argv );
}
