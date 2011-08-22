#include "itktfDifferenceImageFilter.h"
#include "itktfRegression.h"
#include "itkNumericTraitsVectorPixel.h"
#include "itkVectorImage.h"
#include "itkNumericTraitsVariableLengthVectorPixel.h"


class DifferenceImageFilterTest :
  public itk::Regression
{
protected:
  

  // test to instantiate all usage types
  bool Test1(void) const 
  {

    typedef itk::Image< float, 3 >                   FloatImageType;
    typedef itk::Image< double, 3 >                  DoubleImageType;
    typedef itk::Image< std::complex< float >, 3 >   ComplexImageType;
    typedef itk::Image< itk::Vector< float, 3 >, 3 > Vector3fImageType;
    typedef itk::Image< itk::Vector< float, 2 >, 2 > Vector2dImage2DType;
    typedef itk::VectorImage< double >               VectorImageType;

    typedef itk::tf::DifferenceImageFilter< FloatImageType, FloatImageType > FloatDifferenceImageFilterType;
    typedef itk::tf::DifferenceImageFilter< itk::Image< std::complex< float>, 3>, itk::Image< std::complex< float>, 3 > > ComplexDifferenceImageFilterType;
    typedef itk::tf::DifferenceImageFilter< itk::Image< itk::Vector< float, 3 >, 3>, itk::Image< itk::Vector< float, 3 >, 3 > > VectorDifferenceImageFilterType;
    typedef itk::tf::DifferenceImageFilter< itk::Image< itk::Vector< double, 3 >, 2>, itk::Image< itk::Vector< float, 3 >, 2 > > Vector2DifferenceImageFilterType;
    typedef itk::tf::DifferenceImageFilter< itk::VectorImage< double >, itk::VectorImage< double > > VariableLengthVectorDifferenceImageFilterType;

    FloatDifferenceImageFilterType::Pointer f1 = FloatDifferenceImageFilterType::New();
    VectorDifferenceImageFilterType::Pointer f2 = VectorDifferenceImageFilterType::New();
    Vector2DifferenceImageFilterType::Pointer f3 = Vector2DifferenceImageFilterType::New();
    VariableLengthVectorDifferenceImageFilterType::Pointer f4 = VariableLengthVectorDifferenceImageFilterType::New();
    
    return true;
  }

  virtual int Test( int argc, char *argv[] )
  {
    bool success = true;

    success &= this->Test1();
    
    
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
  }
};


int main( int argc, char *argv[] )
{
  DifferenceImageFilterTest test; 
  return test.Main( argc, argv ) ;

}
