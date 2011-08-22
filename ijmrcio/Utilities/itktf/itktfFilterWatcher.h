/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFilterWatcher.h,v $
  Language:  C++
  Date:      $Date: 2007-01-29 14:42:11 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itktfFilterWatcher_h
#define _itktfFilterWatcher_h

#include "itkCommand.h"
#include "itkProcessObject.h"
#include <time.h>

namespace itk 
{
namespace tf 
{

// The following class is a convenience to watch the progress of a
// filter, and it's output has eliminated pointers and other system
// dependent output, so that the text output can be used as a testing
// measurement
class FilterWatcher
{
public:
  FilterWatcher(itk::ProcessObject* o, std::ostream &_os = std::cout, const char *comment="")
    : os(_os)
  {
     m_Process = o; m_Steps = 0; m_Comment = comment;
    itk::SimpleMemberCommand<FilterWatcher>::Pointer startFilterCommand;
    itk::SimpleMemberCommand<FilterWatcher>::Pointer endFilterCommand;
    itk::SimpleMemberCommand<FilterWatcher>::Pointer progressFilterCommand;
    itk::SimpleMemberCommand<FilterWatcher>::Pointer iterationFilterCommand;
    itk::SimpleMemberCommand<FilterWatcher>::Pointer abortFilterCommand;
  
    startFilterCommand =    itk::SimpleMemberCommand<FilterWatcher>::New();
    endFilterCommand =      itk::SimpleMemberCommand<FilterWatcher>::New();
    progressFilterCommand = itk::SimpleMemberCommand<FilterWatcher>::New();
    iterationFilterCommand = itk::SimpleMemberCommand<FilterWatcher>::New();
    abortFilterCommand = itk::SimpleMemberCommand<FilterWatcher>::New();

    startFilterCommand->SetCallbackFunction(this, &FilterWatcher::StartFilter);
    endFilterCommand->SetCallbackFunction(this, &FilterWatcher::EndFilter);
    progressFilterCommand->SetCallbackFunction(this, &FilterWatcher::ShowProgress);
    iterationFilterCommand->SetCallbackFunction(this, &FilterWatcher::ShowIteration);
    abortFilterCommand->SetCallbackFunction(this, &FilterWatcher::ShowAbort);

    m_Process->AddObserver(itk::StartEvent(), startFilterCommand);
    m_Process->AddObserver(itk::EndEvent(), endFilterCommand);
    m_Process->AddObserver(itk::ProgressEvent(), progressFilterCommand);
    m_Process->AddObserver(itk::IterationEvent(), iterationFilterCommand);
    m_Process->AddObserver(itk::AbortEvent(), abortFilterCommand);
  }

  virtual ~FilterWatcher() {}

  virtual void ShowProgress() {}

  virtual void ShowAbort()
  {
    os << std::endl << "      ABORT" << std::endl << std::flush;
  }

  virtual void ShowIteration()
  {
    os << " # " << std::flush;
    m_Iterations++;
  }

  virtual void StartFilter()
  {
    m_Steps = 0;
    m_Iterations = 0;
    os << "-------- Start " << m_Process->GetNameOfClass()
       << " \"" << m_Comment << "\" "
       << std::flush;
  }

  const char *GetNameOfClass () {return m_Process->GetNameOfClass();}

  virtual void EndFilter()
  {  
    os << std::endl 
       << "-------- End " << m_Process->GetNameOfClass()
       << " \"" << m_Comment << "\" " << std::endl ;
    }
  
protected:
  int m_Steps;
  int m_Iterations;
  std::string m_Comment;
  itk::ProcessObject::Pointer m_Process;

  std::ostream &os;

private:
  FilterWatcher(); // Purposely not implemented
};

}
}
#endif
