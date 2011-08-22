#include "itktfRegression.h"


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

namespace itk 
{

class Test3 :
  public itk::Regression
{
protected:
  virtual int Test( int argc, char *argv[] )
  {
    
    typedef itk::Image<unsigned char,3> ImageType;
    
    fileutil::PathName inputFilename = fileutil::PathName( ITK_REGRESSION_INPUT ).Append("vm_head_coronal.png");
    fileutil::PathName outputFilename = fileutil::PathName( ITK_REGRESSION_OUTPUT ).Append("itktfTest3.png");

    // Now generate a real image
    typedef itk::ImageFileReader<ImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( inputFilename.GetPathName() );

    typedef itk::ImageFileWriter<ImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outputFilename.GetPathName() );
    writer->SetInput( reader->GetOutput() );
    writer->Update();

    this->MeasurementInsightFileImage( outputFilename.GetPathName(), "Pipelined Image" );

    return EXIT_SUCCESS;
  }
};

}

int main( int argc, char *argv[] )
{
  itk::Test3 test; 
  return test.Main( argc, argv );
}
