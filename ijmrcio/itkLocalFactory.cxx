#include "itkLocalFactory.h"
#include "itkVTKImageIO.h"
#include "itkMRCImageIO.h"
#include "itkVersion.h"
#include "itkImageIOFactory.h"

namespace itk 
{
namespace Local 
{
  /** Class methods used to interface with the registered factories. */
const char* LocalFactory::GetITKSourceVersion(void) const
    {
    return ITK_SOURCE_VERSION;
    }

  const char* LocalFactory::GetDescription(void) const
    {
    return "MRC ImageIO Factory, allows the loading of MRC images into insight";
    }

  LocalFactory::LocalFactory() {

    // first make sure all built in factories are registered
    itk::ImageIOFactory::RegisterBuiltInFactories();

    // then disable VTK factory    
    std::list<ObjectFactoryBase*> fs = itk::ObjectFactoryBase::GetRegisteredFactories();
    for ( std::list<ObjectFactoryBase*>::iterator i = fs.begin(); i != fs.end(); ++i)
      {
      (*i)->SetEnableFlag( false, "itkImageIOBase", "itkVTKImageIO" );
      }
    

    this->RegisterOverride("itkImageIOBase",
                           "itkMRCImageIO",
                           "MRC Image IO",
                           1,
                           CreateObjectFunction<Local::MRCImageIO>::New());


    this->RegisterOverride("itkImageIOBase",
                           "itkLocalVTKImageIO", 
                           "VTK Image IO",
                           1,
                           CreateObjectFunction<Local::VTKImageIO>::New());
  };

  LocalFactory::~LocalFactory() {
  };

} // namespace local
} // namespace itk
