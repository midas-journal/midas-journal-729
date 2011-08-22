#include "tfRegressionTest.h"

#include "tfTestSupport.h"

namespace testutil {
class tfRegressionTestTest4RegressionTest :
  public testutil::RegressionTest 
{
public:
  tfRegressionTestTest4RegressionTest(void) {    
    fileutil::PathList pathList;
    pathList.push_back( fileutil::PathName( TF_DATA_BASELINE ) );        
    this->SetInFileSearchPath(pathList);
  }
protected:
  virtual int Test( int argc, char *argv[] )
  {

    this->MeasurementFileTextPlain( fileutil::PathName(TF_DATA_BASELINE).Append("plain.txt").GetPathName(), "FileTextPlain" );
  
// these are currently not supported yet...  
//    this->MeasurementFileImagePNG( fileutil::PathName(TF_DATA_BASELINE).Append("vm_head_coronal.png").GetPathName(), "FileImagePNG" );
//    this->MeasurementFileImageJPEG( fileutil::PathName(TF_DATA_BASELINE).Append("vm_head_coronal.jpg").GetPathName(), "FileImageJPEG" );

    return EXIT_SUCCESS;
  }

};
}

int tfRegressionTestTest4( int argc, char *argv[] )
{
  testutil::tfRegressionTestTest4RegressionTest test; 
  int ret = test.Main( argc, argv );

  return ret;
}
