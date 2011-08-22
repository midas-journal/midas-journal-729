#include "itksys/Process.h"

#include <iostream>

#include "TestingFramework/tfRegressionTest.h"


// this program takes another executable as an argument. It will then
// run that command with the remaining arguments and capture the
// standard output, which will then be used as a plain text measurement
class tfPipeToXMLRegressionTest :
  public testutil::RegressionTest 
{
public:
  tfPipeToXMLRegressionTest(void) {};
protected:
  virtual int Test( int argc, char *argv[] )
  {
    // we expect the input arguments to be tfPipeToXML cmdtorun arg1
    // ... argN
    std::cout << "Redirecting output from: \"";
    const char **cmd = new const char*[argc];
    for ( int i = 1; i < argc; ++i ) 
      {
      cmd[i-1] = argv[i];
      if (i != 1)
        std::cout << " ";
      std::cout << argv[i];      
      }
    cmd[argc - 1] = 0;
    std::cout << "\"" << std::endl;

    
    char* data = 0;
    int length = 0;
    int ret = -1;

    std::string outputString;

    itksysProcess* kp =  itksysProcess_New();

    itksysProcess_SetCommand(kp, cmd);

  
    itksysProcess_Execute(kp);

    int p;
    while((p = itksysProcess_WaitForData(kp, &data, &length, 0 )))
      {
      outputString += std::string( data, length );
      }
  
    itksysProcess_WaitForExit(kp, 0);

    this->MeasurementTextPlain( outputString, "stdout" );

    switch (itksysProcess_GetState(kp))
      {
      case itksysProcess_State_Starting:
        std::cerr << "No process has been executed." << std::endl; 
        break;
      case itksysProcess_State_Executing:
        std::cerr << "The process is still executing." << std::endl;; 
        break;
      case itksysProcess_State_Expired:
        std::cerr << "Child was killed when timeout expired." << std::endl;
        break;
      case itksysProcess_State_Exited:
        ret = itksysProcess_GetExitValue(kp); 
        break;
      case itksysProcess_State_Killed:
        std::cerr << "Child was killed by parent." << std::endl; 
        break;
      case itksysProcess_State_Exception:
        std::cerr << "Child terminated abnormally: " << itksysProcess_GetExceptionString(kp) << std::endl;; 
        break;
      case itksysProcess_State_Disowned:
        std::cerr << "Child was disowned." << std::endl; 
        break;
      case itksysProcess_State_Error:
        std::cerr << "Error in administrating child process: [" << itksysProcess_GetErrorString(kp) << "]" << std::endl; 
        break;
      };
    
    itksysProcess_Delete(kp);

    
    return ret;

  }
};
  



int main( int argc, char *argv[] ) 
{
  
  tfPipeToXMLRegressionTest test; 
  return  test.Main( argc, argv );

}
