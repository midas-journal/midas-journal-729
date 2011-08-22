#include "tfRegressionTest.h"

#include "tfTestSupport.h"

class tfRegressionTestOutputTest1RegressionTest :
  public testutil::RegressionTest 
{
public:
  tfRegressionTestOutputTest1RegressionTest(void) {}
protected:
  virtual int Test( int argc, char *argv[] )
  {
    this->MeasurementTextString( "string text value", "TextString" );
    this->MeasurementTextPlain( "this is\n some plain text\n", "TextPlain" );
    
    this->MeasurementNumericInteger( long(0), "NumericInteger" );
    this->MeasurementNumericFloat( float(1.1), "NumericFloat" );
    this->MeasurementNumericDouble( double(30.0/9.0), "NumericDouble" );
    this->MeasurementNumericBoolean( true, "NumericBoolean" );

    return EXIT_SUCCESS;
  }

};
  

int tfRegressionTestOutputTest1( int argc, char *argv[] )
{
  tfRegressionTestOutputTest1RegressionTest test; 
  int ret = test.Main( argc, argv ) ;

  if ( 6 == ret  ) 
    {
    return EXIT_SUCCESS;
    }
  else 
    {
    std::cerr << "Expected regression test to return 6 measurements not matching" << std::endl;
    return EXIT_FAILURE;
    }
}
