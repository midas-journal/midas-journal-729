#include "tfRegressionTest.h"

#include "tfTestSupport.h"

class tfRegressionTestTest1RegressionTest :
  public testutil::RegressionTest 
{
public:
  tfRegressionTestTest1RegressionTest(void) {}
protected:
  virtual int Test( int argc, char *argv[] )
  {
    return EXIT_SUCCESS;
  }
};
  

int tfRegressionTestTest1( int argc, char *argv[] )
{
  tfRegressionTestTest1RegressionTest test; 
  return  test.Main( argc, argv );
}
