
SET ( TESTINGFRAMEWORK_XML_BASELINE ${CMAKE_CURRENT_SOURCE_DIR}/xml )
SET ( TESTINGFRAMEWORK_DATA_BASELINE ${CMAKE_CURRENT_SOURCE_DIR}/data )

CONFIGURE_FILE("${TESTINGFRAMEWORK_SOURCE_DIR}/tests/tfTestSupport.h.in"
  "${TESTINGFRAMEWORK_BINARY_DIR}/tfTestSupport.h" IMEDIATE) 

#create the testing file and list of tests
CREATE_TEST_SOURCELIST ( TestingFrameworkTests1_SRC
  tfTests1.cxx
  tfHeaderTest.cxx  
)

# create the testing file and list of tests
# these tests utilize XML comparison
CREATE_TEST_SOURCELIST ( TestingFrameworkTests2_SRC
  tfTests2.cxx    
  tfRegressionTestOutputTest1.cxx
  tfRegressionTestOutputTest2.cxx
  tfRegressionTestTest1.cxx
  tfRegressionTestTest2.cxx
  tfRegressionTestTest3.cxx
  tfRegressionTestTest4.cxx
  tfRegressionTestArgumentsTest.cxx
)

INCLUDE_DIRECTORIES( ${TESTINGFRAMEWORK_SOURCE_DIR} )

ADD_EXECUTABLE( tfTests1 ${TestingFrameworkTests1_SRC} )
TARGET_LINK_LIBRARIES (tfTests1 testingframework fileutil )

ADD_EXECUTABLE( tfTests2 ${TestingFrameworkTests2_SRC} )
TARGET_LINK_LIBRARIES (tfTests2 testingframework fileutil )


SET ( TESTINGFRAMEWORK_TEST1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tfTests1 )

SET ( TestsToRun ${TestingFrameworkTests1_SRC} )
REMOVE ( TestsToRun tfTests1.cxx ) 
FOREACH ( test ${TestsToRun} )
  GET_FILENAME_COMPONENT ( TName ${test} NAME_WE )
  ADD_TEST ( ${TName} ${TESTINGFRAMEWORK_TEST1} ${TName} )
ENDFOREACH ( test )


SET ( TESTINGFRAMEWORK_TEST2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tfTests2 )

SET ( TestsToRun ${TestingFrameworkTests2_SRC} )
REMOVE ( TestsToRun tfTests2.cxx ) 
REMOVE ( TestsToRun tfRegressionTestArgumentsTest.cxx )

FOREACH ( test ${TestsToRun} )
  GET_FILENAME_COMPONENT ( TName ${test} NAME_WE )

  # This is the line which executes the test it also set the input XML file
  ADD_TEST ( ${TName} ${TESTINGFRAMEWORK_TEST2} ${TName} -I ${TESTINGFRAMEWORK_XML_BASELINE}/${TName}.xml )
ENDFOREACH ( test )

ADD_TEST ( tfRegressionTestArgumentsTest1 ${TESTINGFRAMEWORK_TEST2}  tfRegressionTestArgumentsTest -I ${TESTINGFRAMEWORK_XML_BASELINE}/tfRegressionTestArgumentsTest1.xml )
ADD_TEST ( tfRegressionTestArgumentsTest2 ${TESTINGFRAMEWORK_TEST2}  tfRegressionTestArgumentsTest -I ${TESTINGFRAMEWORK_XML_BASELINE}/tfRegressionTestArgumentsTest2.xml one)
ADD_TEST ( tfRegressionTestArgumentsTest3 ${TESTINGFRAMEWORK_TEST2}  tfRegressionTestArgumentsTest -I ${TESTINGFRAMEWORK_XML_BASELINE}/tfRegressionTestArgumentsTest3.xml one two)
ADD_TEST ( tfRegressionTestArgumentsTest4 ${TESTINGFRAMEWORK_TEST2}  tfRegressionTestArgumentsTest -I ${TESTINGFRAMEWORK_XML_BASELINE}/tfRegressionTestArgumentsTest4.xml -R 1)
ADD_TEST ( tfRegressionTestArgumentsTest5 ${TESTINGFRAMEWORK_TEST2}  tfRegressionTestArgumentsTest -I ${TESTINGFRAMEWORK_XML_BASELINE}/tfRegressionTestArgumentsTest5.xml -R 1 one )
ADD_TEST ( tfRegressionTestArgumentsTest6 ${TESTINGFRAMEWORK_TEST2}  tfRegressionTestArgumentsTest -I ${TESTINGFRAMEWORK_XML_BASELINE}/tfRegressionTestArgumentsTest6.xml -R 1 one two )
ADD_TEST ( tfRegressionTestArgumentsTest7 ${TESTINGFRAMEWORK_TEST2}  tfRegressionTestArgumentsTest -I ${TESTINGFRAMEWORK_XML_BASELINE}/tfRegressionTestArgumentsTest7.xml one -R 1 three  )
ADD_TEST ( tfRegressionTestArgumentsTest8 ${TESTINGFRAMEWORK_TEST2}  tfRegressionTestArgumentsTest -P ${TESTINGFRAMEWORK_DATA_BASELINE} -I ${TESTINGFRAMEWORK_XML_BASELINE}/tfRegressionTestArgumentsTest8.xml one two  )