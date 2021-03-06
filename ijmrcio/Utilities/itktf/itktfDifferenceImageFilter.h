/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDifferenceImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2009-04-25 12:24:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itktfDifferenceImageFilter_h
#define __itktfDifferenceImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkNumericTraits.h"
#include "itkArray.h"
#include "itkVariableLengthVector.h"

namespace itk
{

namespace Math
{

template <typename TPixelType>
TPixelType ZeroValue( const TPixelType & )
{
  return itk::NumericTraits<TPixelType>::Zero; 
}


template<typename T>
itk::VariableLengthVector<T>
ZeroValue( const itk::VariableLengthVector<T> &v )
{
  return  itk::NumericTraits<itk::VariableLengthVector<T> >::Zero(v); 
}

template<typename TPixelType>
typename NumericTraits<TPixelType>::ScalarRealType 
SquaredTwoNorm( const TPixelType &v)
{
  return v*v;
}

template<typename T, unsigned int D>
typename NumericTraits< FixedArray<T,D> >::ScalarRealType 
SquaredTwoNorm( const FixedArray<T,D> &v)
{
  return std::inner_product( v.Begin(), v.End(), v.Begin(), v.End(), NumericTraits<T>::Zero );
}

template<typename T>
typename itk::NumericTraits<std::complex<T> >::ScalarRealType
SquaredTwoNorm( const std::complex<T> &v) {
  return norm( v );
}


template<typename T>
typename itk::NumericTraits<
  itk::VariableLengthVector<T> >::ScalarRealType
SquaredTwoNorm( const itk::VariableLengthVector<T> &v) {
  return v.GetSquaredNorm();
}


}

namespace tf
{
  
/** \class DifferenceImageFilter
 * \brief Implements comparison between two images.
 *
 * This filter is used by the testing system to compute the difference between
 * a valid image and an image produced by the test. The comparison value is
 * computed by visiting all the pixels in the baseline images and comparing
 * their values with the pixel values in the neighborhood of the homologous
 * pixel in the other image.
 * 
 * \ingroup IntensityImageFilters   Multithreaded
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT DifferenceImageFilter :
    public ImageToImageFilter<TInputImage, TOutputImage> 
{
public:
  /** Standard class typedefs. */
  typedef DifferenceImageFilter                           Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>    Superclass;
  typedef SmartPointer<Self>                              Pointer;
  typedef SmartPointer<const Self>                        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DifferenceImageFilter, ImageToImageFilter);

  /** Some convenient typedefs. */
  typedef TInputImage                            InputImageType;
  typedef TOutputImage                           OutputImageType;
  typedef typename OutputImageType::PixelType    OutputPixelType;
  typedef typename OutputImageType::RegionType   OutputImageRegionType;
  typedef typename InputImageType::SizeValueType SizeValueType;

  typedef typename NumericTraits<OutputPixelType>::RealType       RealType;
  typedef typename NumericTraits<OutputPixelType>::ScalarRealType ScalarRealType;
  typedef typename NumericTraits<RealType>::AccumulateType        AccumulateType;
 
  /** Set the valid image input.  This will be input 0.  */
  virtual void SetValidInput(const InputImageType* validImage);
  
  /** Set the test image input.  This will be input 1.  */
  virtual void SetTestInput(const InputImageType* testImage);
  
  /** Set/Get the maximum distance away to look for a matching pixel.
      Default is 0. */
  itkSetMacro(ToleranceRadius, unsigned int);
  itkGetConstMacro(ToleranceRadius, unsigned int);
  
  /** Set/Get the minimum threshold for pixels to be different.
      Default is 0. */
  itkSetMacro(MagnitudeThreshold, ScalarRealType);
  itkGetConstMacro(MagnitudeThreshold, ScalarRealType);
  
  /** Set/Get ignore boundary pixels.  Useful when resampling may have
   *    introduced difference pixel values along the image edge 
   *    Default = false */
  itkSetMacro(IgnoreBoundaryPixels, bool);
  itkGetConstMacro(IgnoreBoundaryPixels, bool);
  
  /** Get parameters of the difference image after execution.  */
  itkGetConstMacro(MeanMagnitude, ScalarRealType);
  itkGetConstMacro(TotalMagnitude, ScalarRealType);
  itkGetConstMacro(NumberOfPixelsWithDifferences, SizeValueType);
  
protected:
  DifferenceImageFilter();
  virtual ~DifferenceImageFilter() {}
  
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** DifferenceImageFilter can be implemented as a multithreaded
   * filter.  Therefore, this implementation provides a
   * ThreadedGenerateData() routine which is called for each
   * processing thread. The output image data is allocated
   * automatically by the superclass prior to calling
   * ThreadedGenerateData().  ThreadedGenerateData can only write to
   * the portion of the output image specified by the parameter
   * "outputRegionForThread"
   *
   * \sa ImageToImageFilter::ThreadedGenerateData(),
   *     ImageToImageFilter::GenerateData()  */
  void ThreadedGenerateData(const OutputImageRegionType& threadRegion,
                            int threadId);

  void BeforeThreadedGenerateData();
  void AfterThreadedGenerateData();
 
  ScalarRealType m_MagnitudeThreshold;
  ScalarRealType m_MeanMagnitude;
  ScalarRealType m_TotalMagnitude;
  SizeValueType  m_NumberOfPixelsWithDifferences;
  unsigned int   m_ToleranceRadius;

  std::vector<ScalarRealType> m_ThreadMagnitudeSum;
  std::vector<SizeValueType>  m_ThreadNumberOfPixels;

private:
  DifferenceImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool m_IgnoreBoundaryPixels;
};

} // end namespace tf
} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_DifferenceImageFilter(_, EXPORT, x, y) namespace itk { \
  _(2(class EXPORT DifferenceImageFilter< ITK_TEMPLATE_2 x >)) \
  namespace Templates { typedef DifferenceImageFilter< ITK_TEMPLATE_2 x > \
                                            DifferenceImageFilter##y; } \
  }

# include "itktfDifferenceImageFilter.txx"


#endif // __itktfDifferenceImageFilter_h
