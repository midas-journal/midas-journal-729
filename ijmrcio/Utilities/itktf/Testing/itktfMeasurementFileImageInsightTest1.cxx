#include "itktfRegression.h"

namespace itk 
{

class MeasurementFileImageInsightTest1 :
  public itk::Regression
{
protected:
  virtual int Test( int argc, char *argv[] )
  {

    this->MeasurementInsightFileImage( fileutil::PathName(ITK_REGRESSION_DATA_BASELINE).Append("vm_head_coronal.png").GetPathName(), "Insight PNG Image" );
    
    this->MeasurementInsightFileImage( fileutil::PathName(ITK_REGRESSION_DATA_BASELINE).Append("vm_head_coronal.jpg").GetPathName(), "Insight JPEG Image" );

    return EXIT_SUCCESS;
  }
};

}

int main( int argc, char *argv[] )
{
  itk::MeasurementFileImageInsightTest1 test; 
  return test.Main( argc, argv ) ;

}
