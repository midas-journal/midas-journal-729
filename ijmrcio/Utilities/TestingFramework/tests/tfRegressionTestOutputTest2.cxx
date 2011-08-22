#include "tfRegressionTest.h"


#include "FileUtilities/PathName.h"

#include "tfTestSupport.h"

namespace testutil {
class tfRegressionTestOutputTest2RegressionTest :
  public RegressionTest 
{
public:
  tfRegressionTestOutputTest2RegressionTest(void) {}
protected:
  virtual int Test( int argc, char *argv[] )
  {
    this->MeasurementFileTextPlain( fileutil::PathName(TF_DATA_BASELINE).Append("plain.txt").GetPathName(), "FileTextPlain" );
    
    this->MeasurementFileImagePNG( fileutil::PathName(TF_DATA_BASELINE).Append("vm_head_coronal.png").GetPathName(), "FileImagePNG" );
    this->MeasurementFileImageJPEG( fileutil::PathName(TF_DATA_BASELINE).Append("vm_head_coronal.jpg").GetPathName(), "FileImageJPEG" );

    return EXIT_SUCCESS;
  }

};
  
}
int tfRegressionTestOutputTest2( int argc, char *argv[] ) 
{
  testutil::tfRegressionTestOutputTest2RegressionTest test; 
  int ret = test.Main( argc, argv ) ;

  if ( 3 == ret  ) 
    {
    return EXIT_SUCCESS;
    }
  else 
    {
    std::cerr << "Expected regression test to return 3 measurements not matching" << std::endl;
    return EXIT_FAILURE;
    }
}
