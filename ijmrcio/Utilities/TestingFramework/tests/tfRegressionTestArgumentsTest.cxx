#include "tfRegressionTest.h"

#include "tfTestSupport.h"

#include <sstream>

class tfRegressionTestArgumentRegressionTest :
  public testutil::RegressionTest 
{
public:
  tfRegressionTestArgumentRegressionTest(void) {}
protected:
  virtual int Test( int argc, char *argv[] )
  {
    
    this->MeasurementNumericInteger( this->GetExpectedReturn(), "ExpectedReturn" );


    for ( int i = 0; i < argc; ++i ) 
      {
      std::ostringstream oss;
      oss << "Argument " << i;
      this->MeasurementTextString( argv[i], oss.str() );
      }
    return EXIT_SUCCESS;
  }
};
  

int tfRegressionTestArgumentsTest( int argc, char *argv[] )
{
  tfRegressionTestArgumentRegressionTest test; 
  return  test.Main( argc, argv );
}
