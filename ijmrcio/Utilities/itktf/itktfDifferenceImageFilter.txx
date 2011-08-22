/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkDifferenceImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2008-10-07 09:09:39 $
  Version:   $Revision: 1.18 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itktfDifferenceImageFilter_txx
#define __itktfDifferenceImageFilter_txx


#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkProgressReporter.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"

namespace itk
{
namespace tf
{

//----------------------------------------------------------------------------
template <class TInputImage, class TOutputImage>
DifferenceImageFilter<TInputImage, TOutputImage>
::DifferenceImageFilter() 
{
  // We require two inputs to execute.
  this->SetNumberOfRequiredInputs(2);
  
  // Set the default MagnitudeThreshold.
  m_MagnitudeThreshold = NumericTraits<ScalarRealType>::Zero;
  
  // Set the default ToleranceRadius.
  m_ToleranceRadius = 0;
  
  // Initialize statistics about difference image.
  m_MeanMagnitude = NumericTraits<ScalarRealType>::Zero;
  m_TotalMagnitude = NumericTraits<ScalarRealType>::Zero;
  m_NumberOfPixelsWithDifferences = 0;
  m_IgnoreBoundaryPixels = false;
}

//----------------------------------------------------------------------------
template<class TInputImage, class TOutputImage>
void 
DifferenceImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "ToleranceRadius: " << m_ToleranceRadius << "\n";  
  os << indent << "MagnitudeThreshold: " << m_MagnitudeThreshold << "\n";
  os << indent << "MeanMagnitude: " << m_MeanMagnitude << "\n";
  os << indent << "TotalMagnitude: " << m_TotalMagnitude << "\n";
  os << indent << "NumberOfPixelsWithDifferences: " 
               << m_NumberOfPixelsWithDifferences << "\n";
  os << indent << "IgnoreBoundaryPixels: " 
               << m_IgnoreBoundaryPixels << "\n";
}

//----------------------------------------------------------------------------
template <class TInputImage, class TOutputImage>
void 
DifferenceImageFilter<TInputImage, TOutputImage>
::SetValidInput(const InputImageType* validImage)
{
  // The valid image should be input 0.
  this->SetInput(0, validImage);
}

//----------------------------------------------------------------------------
template <class TInputImage, class TOutputImage>
void 
DifferenceImageFilter<TInputImage, TOutputImage>
::SetTestInput(const InputImageType* testImage)
{
  // The test image should be input 1.
  this->SetInput(1, testImage);
}

//----------------------------------------------------------------------------
template<class TInputImage, class TOutputImage>
void
DifferenceImageFilter<TInputImage, TOutputImage>
::BeforeThreadedGenerateData()
{
  int numberOfThreads = this->GetNumberOfThreads();
  
  // Resize the thread temporaries
  m_ThreadMagnitudeSum.resize(numberOfThreads);
  m_ThreadNumberOfPixels.resize(numberOfThreads);
  
  // Initialize the temporaries

  std::fill( m_ThreadMagnitudeSum.begin(), m_ThreadMagnitudeSum.end(), NumericTraits<ScalarRealType>::Zero );
  std::fill( m_ThreadNumberOfPixels.begin(), m_ThreadNumberOfPixels.end(), NumericTraits<SizeValueType>::Zero );

}

//----------------------------------------------------------------------------
template <class TInputImage, class TOutputImage>
void
DifferenceImageFilter<TInputImage, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType &threadRegion, int threadId)
{
  typedef ConstNeighborhoodIterator<InputImageType>   SmartIterator;
  typedef ImageRegionConstIterator<InputImageType>    InputIterator;
  typedef ImageRegionIterator<OutputImageType>        OutputIterator;
  typedef NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType> 
                                                      FacesCalculator;
  typedef typename FacesCalculator::RadiusType        RadiusType;
  typedef typename FacesCalculator::FaceListType      FaceListType;
  typedef typename FaceListType::iterator             FaceListIterator;
  typedef typename InputImageType::PixelType          InputPixelType;
 
  // Prepare standard boundary condition.
  ZeroFluxNeumannBoundaryCondition<InputImageType> nbc;

  // Get a pointer to each image.
  const InputImageType* validImage = this->GetInput(0);
  const InputImageType* testImage = this->GetInput(1);
  OutputImageType* outputPtr = this->GetOutput();
  
  // Create a radius of pixels.
  RadiusType radius;
  radius.Fill(m_ToleranceRadius);
  
  // Find the data-set boundary faces.
  FacesCalculator boundaryCalculator;
  FaceListType faceList = boundaryCalculator(testImage, threadRegion, radius);
  
  // Support progress methods/callbacks.
  ProgressReporter progress(this, threadId, threadRegion.GetNumberOfPixels());
  
  // Process the internal face and each of the boundary faces.
  for (FaceListIterator face = faceList.begin(); face != faceList.end(); ++face)
    { 
    SmartIterator test(radius, testImage, *face); // Iterate over test image.
    InputIterator valid(validImage, *face);       // Iterate over valid image.
    OutputIterator out(outputPtr, *face);         // Iterate over output image.
    if( !test.GetNeedToUseBoundaryCondition() || !m_IgnoreBoundaryPixels )
      {
      test.OverrideBoundaryCondition(&nbc);
    
      for(valid.GoToBegin(), test.GoToBegin(), out.GoToBegin();
          !valid.IsAtEnd();
          ++valid, ++test, ++out)
        {
        // Get the current valid pixel.
        InputPixelType c = valid.Get();
        
        //  Assume a good match - so test center pixel first, for speed
        RealType minimumDifference = static_cast<RealType>(c) - test.GetCenterPixel();
        
        ScalarRealType minimumMagnitude2 = Math::SquaredTwoNorm( minimumDifference );

        // If center pixel isn't good enough, then test the neighborhood
        if( minimumMagnitude2 > m_MagnitudeThreshold * m_MagnitudeThreshold )
          {
          unsigned int neighborhoodSize = test.Size();
          // Find the closest-valued pixel in the neighborhood of the test
          // image.
          for (unsigned int i=0; i < neighborhoodSize; ++i)
            {
            // Use the RealType for the difference to make sure we get the
            // sign.
            RealType difference = static_cast<RealType>(c) - test.GetPixel(i);
            ScalarRealType magnitude2 = Math::SquaredTwoNorm( difference );
            
            // update to the minimum values
            if ( magnitude2 < minimumMagnitude2 )
              {
              minimumDifference = difference;
              minimumMagnitude2 = magnitude2;
              }

            if(minimumMagnitude2 <= m_MagnitudeThreshold * m_MagnitudeThreshold)
              {
              break;
              }
            }
          }
          
        // Check if difference is above threshold.
        if ( minimumMagnitude2 > m_MagnitudeThreshold * m_MagnitudeThreshold )
          {
          // Store the minimum difference value in the output image.
          out.Set( static_cast<OutputPixelType>(minimumDifference) );
          // Update difference image statistics.
          m_ThreadMagnitudeSum[threadId] += vcl_sqrt( minimumMagnitude2 );
          m_ThreadNumberOfPixels[threadId]++;
          }
        else
          {
          // Difference is below threshold.
          out.Set( Math::ZeroValue(c) );
          }
        
        // Update progress.
        progress.CompletedPixel();
        }
      }
    else
      {

      valid.GoToBegin();
      InputPixelType c = valid.Get();

      for(out.GoToBegin(); !out.IsAtEnd(); ++out)
        {
        out.Set( Math::ZeroValue(c));
        progress.CompletedPixel();
        }
      }
    }
}

//----------------------------------------------------------------------------
template <class TInputImage, class TOutputImage>
void
DifferenceImageFilter<TInputImage, TOutputImage>
::AfterThreadedGenerateData()
{
  // Set statistics about difference image.
  m_TotalMagnitude = std::accumulate( m_ThreadMagnitudeSum.begin(), m_ThreadMagnitudeSum.end(), NumericTraits<ScalarRealType>::Zero );
  m_NumberOfPixelsWithDifferences = std::accumulate( m_ThreadNumberOfPixels.begin(), m_ThreadNumberOfPixels.end(), NumericTraits<SizeValueType>::Zero );
  
  // Get the total number of pixels processed in the region.
  // This is different from the m_TotalNumberOfPixels which
  // is the number of pixels that actually have differences
  // above the intensity threshold.
  OutputImageRegionType region = this->GetOutput()->GetRequestedRegion();
  SizeValueType numberOfPixels = region.GetNumberOfPixels();
  
  // Calculate the mean difference.
  m_MeanMagnitude = m_TotalMagnitude / numberOfPixels;
}

} // end namespace tf
} // end namespace itk

#endif //  __itktfDifferenceImageFilter_txx
