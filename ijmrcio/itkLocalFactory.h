#ifndef __itkLocalFactory_h
#define __itkLocalFactory_h


#include "itkObjectFactoryBase.h"

namespace itk
{

namespace Local 
{

class ITK_EXPORT LocalFactory : public ObjectFactoryBase
{
public:
  /** Standard class typedefs. */
  typedef LocalFactory                         Self;
  typedef ObjectFactoryBase                         Superclass;
  typedef SmartPointer<Self>                        Pointer;
  typedef SmartPointer<const Self>                  ConstPointer;

  /** Class methods used to interface with the registered factories. */
  const char* GetITKSourceVersion(void) const;

  const char* GetDescription(void) const;

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(LocalFactory, ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory(void)
    {
    ObjectFactoryBase::RegisterFactory( Self::New() );
    }

protected:
  LocalFactory();

  ~LocalFactory();

 private:
  LocalFactory(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // namespace local
} // namespace itk

#endif  // __itkLocalFactory_h
