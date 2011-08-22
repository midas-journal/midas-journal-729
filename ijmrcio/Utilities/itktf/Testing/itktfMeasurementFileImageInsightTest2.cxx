#include "itktfRegression.h"

namespace itk 
{

class tfitkTest1 :
  public itk::Regression
{
protected:
  virtual int Test( int argc, char *argv[] )
  {

    if (this->GetCompareMode()) 
      this->MeasurementInsightFileImage( fileutil::PathName(ITK_REGRESSION_DATA_BASELINE).Append("vm_head_coronal.png").GetPathName(), "Different Images PNG" );
    else
      this->MeasurementInsightFileImage( fileutil::PathName(ITK_REGRESSION_DATA_BASELINE).Append("vm_head_coronal_diff.png").GetPathName(), "Different Images PNG" );


    if (this->GetCompareMode()) 
      this->MeasurementInsightFileImage( fileutil::PathName(ITK_REGRESSION_DATA_BASELINE).Append("vm_head_coronal.jpg").GetPathName(), "Different Images PNG to JPEG" );
    else
      this->MeasurementInsightFileImage( fileutil::PathName(ITK_REGRESSION_DATA_BASELINE).Append("vm_head_coronal.png").GetPathName(), "Different Images PNG to JPEG" );

    return EXIT_SUCCESS;
  }
};

}

int main( int argc, char *argv[] )
{
  itk::tfitkTest1 test; 
  int ret = test.Main( argc, argv ) ;

  if ( 2 == ret  ) 
    {
    return EXIT_SUCCESS;
    }
  else 
    {
    std::cerr << "Expected regression test to return 2 measurements not matching" << std::endl;
    return EXIT_FAILURE;
    }
}
