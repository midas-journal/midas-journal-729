#ifndef __itkMRCImageIO_h
#define __itkMRCImageIO_h

#include "itkStreamingImageIOBase.h"
#include "itkMRCHeaderObject.h"

namespace itk
{
namespace Local
{

/** \brief An ImageIO class to read the MRC file format.
 *
 * The MRC file format frequently has the extension ".mrc" or
 * ".rec". It is used frequently for electron microscopy and is an
 * emerging standard for cryo-electron tomography and molecular
 * imaging. The format is used to represent 2D, 3D images along with
 * 2D tilt series for tomography.
 *
 * The header of the file can contain important information which can
 * not be represented in an Image. Therefor the header is placed into
 * the MetaDataDictionary of "this". The key to access this is
 * MetaDataHeaderName ( fix me when renamed ).
 * \sa MRCHeaderObject MetaDataDictionary
 *
 * This implementation is designed to support IO Streaming of
 * arbitrary regions.
 *
 * As with all ImageIOs this class is designed to work with
 * ImageFileReader and ImageFileWriter, so its direct use is
 * discouraged.
 * 
 * \sa ImageFileWriter ImageFileReader ImageIOBase
 */
class ITK_EXPORT MRCImageIO 
  : public StreamingImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef MRCImageIO         Self;
  typedef ImageIOBase        Superclass;
  typedef SmartPointer<Self> Pointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MRCImageIO, ImageIOBase);


  // we don't use this methods
  virtual void WriteImageInformation( void ) {};

  //-------- This part of the interface deals with reading data. ------ 

  // See super class for documentation
  virtual bool CanReadFile(const char*);

  // See super class for documentation
  virtual void ReadImageInformation();

  // See super class for documentation
  virtual void Read(void* buffer);

  // -------- This part of the interfaces deals with writing data. -----

  /** \brief Returns true if this ImageIO can write the specified
   * file. 
   * 
   * The methods verifies that the file extension is known to be
   * supported by this class.
   */
  virtual bool CanWriteFile(const char*);


  // see super class for documentation
  virtual void Write(const void* buffer);

  /** \todo Move to itkIOCommon with the other MetaDataDictionary
   * keys, likely rename the symbol to something like
   * ITK_MRCHHeader. (remember to fix class doc too)
   */
  static const char *MetaDataHeaderName;

protected:
  MRCImageIO();
  // ~MRCImageIO(); // default works
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Overloaded to return the actually header size of the file
   * specified. The header must be read before this methods is
   * called.
   */
  virtual SizeType GetHeaderSize( void ) const;

private:

  MRCImageIO(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  // internal methods to update the min and max in the header based on
  // the data, in the image buffer to be written
  template<typename TPixelType>
    void UpdateHeaderWithMinMaxMean( const TPixelType * bufferBegin );
  void UpdateHeaderWithMinMaxMean( const void *buffer );

  // internal methods to update the header object from the ImageIO's
  // set member variables
  void UpdateHeaderFromImageIO( void );
  
  // reimplemented
  void InternalReadImageInformation(std::ifstream& is);
 
  virtual void WriteImageInformation( const void * bufferBegin );


  MRCHeaderObject::Pointer m_MRCHeader;
};


} // namespace Local
} // namespace itk


#endif
