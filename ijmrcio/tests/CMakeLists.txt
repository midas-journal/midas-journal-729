# these file originally came from a project/library called itkLocal 
# I am keeping the nameing here, but changed it in the parent directory.

CMAKE_POLICY(SET CMP0003 NEW)


# output directory 
SET ( ITK_LOCAL_REGRESSION_OUTPUT_DIR "${PROJECT_BINARY_DIR}/Testing/Temporary" )

# directory for baseline xml files
SET ( ITK_LOCAL_REGRESSION_XML_BASELINE "${CMAKE_CURRENT_SOURCE_DIR}/Baseline/XML" )

# directory for baseline image files
SET ( ITK_LOCAL_REGRESSION_DATA_BASELINE "${CMAKE_CURRENT_SOURCE_DIR}/Baseline/Data" )

# directory for input data
SET ( ITK_LOCAL_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Data" )

# I got an error saying this directory didn't exist, so I now create it
MAKE_DIRECTORY( ${ITK_LOCAL_REGRESSION_OUTPUT_DIR} )


CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/itkLocalRegressionSupport.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/itkLocalRegressionSupport.h" IMEDIATE) 



INCLUDE_DIRECTORIES ( "${IJMRCIO_SOURCE_DIR}" ${CMAKE_CURRENT_BINARY_DIR} ${ITK_TF_SOURCE_DIR} ${ITK_TF_BINARY_DIR} )


#create the teting file and list of tests
CREATE_TEST_SOURCELIST ( itkLocalTests_SRC
  itkLocalTests.cxx

# NEW Generic test for file information
  itkImageFileReaderInfoTest.cxx

# NEW Tests specific options for the file format
  itkMRCImageFileIOTest.cxx
  itkVTKImageFileIOTest.cxx

# REUSED Standard ImageIO Test 
  itkImageFileWriterStreamingPastingCompressingTest1.cxx


# ADAPTED
# REUSED ADAPTED 
# This is a combination of existing tests:
# itkImageFileReaderStreamingTest
# itkImageFileReaderStreamingTest2
# itkImageFileWriterStreamingTest2
# itkImageFileWriterStreamingTest1.cxx  
  itkImageFileStreamingTest.cxx

  itkImageFileWriterPastingTest2.cxx


)



ADD_EXECUTABLE( itkLocalTests ${itkLocalTests_SRC} )
TARGET_LINK_LIBRARIES ( itkLocalTests ITKIO  itkIJMRCIO testingframework itktf)
SET ( ITK_LOCAL_TESTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itkLocalTests )


######################################### 
# IO tests

ADD_TEST(itkImageFileReaderInfoTest_MRC1 ${ITK_LOCAL_TESTS}
  itkImageFileReaderInfoTest 
  -I ${ITK_LOCAL_REGRESSION_XML_BASELINE}/itkImageFileReaderInfoTest_MRC1.xml
  ${ITK_LOCAL_DATA_DIR}/tilt_series.mrc 
  )

ADD_TEST(itkImageFileReaderInfoTest_VTK1 ${ITK_LOCAL_TESTS}
  itkImageFileReaderInfoTest 
  -I ${ITK_LOCAL_REGRESSION_XML_BASELINE}/itkImageFileReaderInfoTest_VTK1.xml
  ${ITK_LOCAL_DATA_DIR}/HeadMRVolume.vtk 
  )

ADD_TEST(itkMRCImageFileIOTest ${ITK_LOCAL_TESTS}
  itkMRCImageFileIOTest 
  -I ${ITK_LOCAL_REGRESSION_XML_BASELINE}/itkMRCImageFileIOTest.xml
  ${ITK_LOCAL_REGRESSION_OUTPUT_DIR}
  )

ADD_TEST(itkVTKImageFileIOTest ${ITK_LOCAL_TESTS}
  itkVTKImageFileIOTest 
  -I ${ITK_LOCAL_REGRESSION_XML_BASELINE}/itkVTKImageFileIOTest.xml
  ${ITK_LOCAL_REGRESSION_OUTPUT_DIR}
  )

# The following is taking from Testing/Code/IO/CMakeLists.txt
##################

# this tests all combinations of streaming pasting and compression
ADD_TEST(itkImageFileWriterStreamingPastingCompressingTest_MRC  ${ITK_LOCAL_TESTS}
  itkImageFileWriterStreamingPastingCompressingTest1
            ${ITK_LOCAL_DATA_DIR}/tilt_series.mrc
            ${ITK_LOCAL_REGRESSION_OUTPUT_DIR}/itkImageFileWriterStreamingPastingCompressingTest mrc 0 0 0 0 0 0 0 0           
            )

ADD_TEST(itkImageFileWriterStreamingPastingCompressingTest_VTK  ${ITK_LOCAL_TESTS}
  itkImageFileWriterStreamingPastingCompressingTest1
            ${ITK_LOCAL_DATA_DIR}/HeadMRVolume.vtk
            ${ITK_LOCAL_REGRESSION_OUTPUT_DIR}/itkImageFileWriterStreamingPastingCompressingTest vtk 0 0 0 0 0 0 0 0           
            )


#This it the old itkImageFileWriterPastingTest2 ported to the XML testing framework
ADD_TEST(itkImageFileWriterPastingTest2_MRC ${ITK_LOCAL_TESTS}
  itkImageFileWriterPastingTest2
  -I ${ITK_LOCAL_REGRESSION_XML_BASELINE}/itkImageFileWriterPastingTest2_MRC.xml
  mrc
  ${ITK_LOCAL_REGRESSION_OUTPUT_DIR}
  ${ITK_LOCAL_DATA_DIR}/tilt_series.mrc
  ${ITK_LOCAL_DATA_DIR}/tilt_series.mrc  
  ${ITK_LOCAL_DATA_DIR}/HeadMRVolume.mrc
  )

ADD_TEST(itkImageFileWriterPastingTest2_VTK ${ITK_LOCAL_TESTS}
  itkImageFileWriterPastingTest2
  -I ${ITK_LOCAL_REGRESSION_XML_BASELINE}/itkImageFileWriterPastingTest2_VTK.xml
  vtk
  ${ITK_LOCAL_REGRESSION_OUTPUT_DIR}
  ${ITK_LOCAL_DATA_DIR}/HeadMRVolume.vtk
  ${ITK_LOCAL_DATA_DIR}/HeadMRVolume.vtk
  ${ITK_LOCAL_DATA_DIR}/vol_ascii.vtk
  )


#This is a combination of of itkImageFileReaderStreamingTest and itkImageFileReaderStreamingTest2
ADD_TEST(itkImageFileStreamingTest_VTK ${ITK_LOCAL_TESTS}
  itkImageFileStreamingTest
  -I  ${ITK_LOCAL_REGRESSION_XML_BASELINE}/itkImageFileStreamingTest_VTK.xml
  ${ITK_LOCAL_DATA_DIR}/HeadMRVolume.vtk
  )
ADD_TEST(itkImageFileStreamingTest_MRC ${ITK_LOCAL_TESTS}
  itkImageFileStreamingTest
  -I  ${ITK_LOCAL_REGRESSION_XML_BASELINE}/itkImageFileStreamingTest_MRC.xml
  ${ITK_LOCAL_DATA_DIR}/tilt_series.mrc
  ) 