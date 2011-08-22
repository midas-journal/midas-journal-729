#include "tfRegressionTest.h"

#include "tfTestSupport.h"
#include <limits>

class tfRegressionTestTest2RegressionTest :
  public testutil::RegressionTest 
{
public:
  tfRegressionTestTest2RegressionTest(void) {}
protected:
  virtual int Test( int argc, char *argv[] )
  {
    this->MeasurementTextString( "string text value", "TextString" );
    this->MeasurementTextPlain( "this is\n some plain text\n", "TextPlain" );

    this->MeasurementNumericInteger( long(0), "NumericInteger" );
    this->MeasurementNumericFloat( float(1.1), "NumericFloat" );

    this->SetRelativeTolerance( std::numeric_limits<double>::epsilon()*64 );
    this->MeasurementNumericDouble( double(20.0/9.0), "NumericDouble" );
    this->MeasurementNumericBoolean( true, "NumericBoolean" );
    

    return EXIT_SUCCESS;
  }

};
  

int tfRegressionTestTest2( int argc, char *argv[] )
{
  tfRegressionTestTest2RegressionTest test; 
  return  test.Main( argc, argv );
}
