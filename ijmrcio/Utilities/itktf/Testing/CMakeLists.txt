
IF(COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

ADD_EXECUTABLE( itktfDifferenceImageFilterTest itktfDifferenceImageFilterTest.cxx )
TARGET_LINK_LIBRARIES( itktfDifferenceImageFilterTest testingframework itktf )

ADD_EXECUTABLE( itktfMeasurementFileImageInsightTest1 itktfMeasurementFileImageInsightTest1.cxx )
TARGET_LINK_LIBRARIES( itktfMeasurementFileImageInsightTest1 testingframework ITKBasicFilters ITKIO itktf )
ADD_TEST( itktfMeasurementFileImageInsightTest1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itktfMeasurementFileImageInsightTest1 -I ${ITK_REGRESSION_XML_BASELINE}/itktfMeasurementFileImageInsightTest1.xml )


ADD_EXECUTABLE( itktfMeasurementFileImageInsightTest2 itktfMeasurementFileImageInsightTest2.cxx )
TARGET_LINK_LIBRARIES( itktfMeasurementFileImageInsightTest2 testingframework ITKBasicFilters ITKIO itktf )
ADD_TEST( itktfMeasurementFileImageInsightTest2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itktfMeasurementFileImageInsightTest2 -I ${ITK_REGRESSION_XML_BASELINE}/itktfMeasurementFileImageInsightTest2.xml )


ADD_EXECUTABLE( itktfMeasurementFileImageInsightTest3 itktfMeasurementFileImageInsightTest3.cxx )
TARGET_LINK_LIBRARIES( itktfMeasurementFileImageInsightTest3 testingframework ITKBasicFilters ITKIO itktf )
ADD_TEST( itktfMeasurementFileImageInsightTest3 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itktfMeasurementFileImageInsightTest3 -I ${ITK_REGRESSION_XML_BASELINE}/itktfMeasurementFileImageInsightTest3.xml )


ADD_EXECUTABLE( itktfMeasurementFileImageInsightTest4 itktfMeasurementFileImageInsightTest4.cxx )
TARGET_LINK_LIBRARIES( itktfMeasurementFileImageInsightTest4 testingframework ITKBasicFilters ITKIO itktf )
ADD_TEST( itktfMeasurementFileImageInsightTest4 
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itktfMeasurementFileImageInsightTest4 
  -I ${ITK_REGRESSION_XML_BASELINE}/itktfMeasurementFileImageInsightTest4.xml 
  ${ITK_REGRESSION_DATA_BASELINE}/vm_head_coronal.jpg
  ${ITK_REGRESSION_OUTPUT_DIR}/itktfMeasurementFileImageInsightTest4.mha
  )

ADD_EXECUTABLE( itktfMeasurementInsightRegionTest itktfMeasurementInsightRegionTest.cxx  )
TARGET_LINK_LIBRARIES( itktfMeasurementInsightRegionTest testingframework ITKBasicFilters ITKIO itktf )
ADD_TEST( itktfMeasurementInsightRegionTest 
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/itktfMeasurementInsightRegionTest 
  -I ${ITK_REGRESSION_XML_BASELINE}/itktfMeasurementInsightRegionTest.xml
  ${ITK_REGRESSION_DATA_BASELINE}/vm_head_coronal.png )
