PROJECT ( IJMRCIO )

CMAKE_MINIMUM_REQUIRED ( VERSION 2.4 )

INCLUDE( ${CMAKE_SOURCE_DIR}/IJMacros.cmake )



#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)

# add my utilities
INCLUDE_DIRECTORIES ( "${PROJECT_SOURCE_DIR}/Utilities" "${PROJECT_BINARY_DIR}/Utilities" "${PROJECT_BINARY_DIR}")
ADD_SUBDIRECTORY ( Utilities/FileUtilities )
ADD_SUBDIRECTORY ( Utilities/TestingFramework )
ADD_SUBDIRECTORY ( Utilities/itktf )


OPTION(BUILD_SHARED_LIBS "Build IJMRCIO with shared libraries." OFF)


#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
SET(Required_Packages
ITK
)


#this foreach loads all of the packages that you specified as required.
#It shouldn't need to be modified.
FOREACH(Package ${Required_Packages})
  LOADPACKAGE(${Package})
ENDFOREACH(Package)

ADD_SUBDIRECTORY( tests )

SET( IJMRCIO_SRC 
  itkMRCHeaderObject.cxx
  itkMRCImageIO.cxx
  itkLocalFactory.cxx 
  itkVTKImageIO.cxx
  itkStreamingImageIOBase.cxx
  )

ADD_LIBRARY( itkIJMRCIO ${IJMRCIO_SRC} )
TARGET_LINK_LIBRARIES ( itkIJMRCIO ITKCommon ITKIO )
