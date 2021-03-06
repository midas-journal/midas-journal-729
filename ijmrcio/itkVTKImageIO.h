/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVTKImageIO.h,v $
  Language:  C++
  Date:      $Date: 2007-03-22 14:28:53 $
  Version:   $Revision: 1.16 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVTKImageIO_h
#define __itkVTKImageIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <fstream>
#include "itkStreamingImageIOBase.h"


namespace itk
{
namespace Local
{

/** \class VTKImageIO
 *
 *  \brief ImageIO class for reading VTK images
 *
 * \ingroup IOFilters
 *
 */
class ITK_EXPORT VTKImageIO 
  : public StreamingImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef VTKImageIO         Self;
  typedef ImageIOBase        Superclass;
  typedef SmartPointer<Self> Pointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VTKImageIO, Superclass);

  /*-------- This part of the interface deals with reading data. ------ */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanReadFile(const char*);
  
  /** Set the spacing and dimesion information for the current filename. */
  virtual void ReadImageInformation();
  
  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void* buffer);

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  virtual bool CanWriteFile(const char*);

  /** Writes the spacing and dimentions of the image.
   * Assumes SetFileName has been called with a valid file name. */
  virtual void WriteImageInformation() {};
  
  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegion has been set properly. */
  virtual void Write(const void* buffer);

  
  /** returns the header size, if it is unknown it will return 0 */
  virtual SizeType GetHeaderSize() const { return this->m_HeaderSize; }

protected:
  VTKImageIO();
  ~VTKImageIO();
  void PrintSelf(std::ostream& os, Indent indent) const;

  void InternalReadImageInformation(std::ifstream& file);
  void WriteImageInformation(const void* buffer);
  
  void ReadHeaderSize( std::ifstream& file );
  
private:
  VTKImageIO(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  void SetPixelTypeFromString( const std::string & pixelType);

  SizeType m_HeaderSize;
};

} // end namespace Local
} // end namespace itk

#endif // __itkVTKImageIO_h
