//  $Id: FileUtil.cpp,v 1.6 2005-06-17 21:02:59 blowek1 Exp $
// ===========================================================================
//
//                            PUBLIC DOMAIN NOTICE
//            Office of High Performance Computing and Communications
//
//  This software/database is a "United States Government Work" under the
//  terms of the United States Copyright Act.  It was written as part of
//  the author's official duties as a United States Government employee and
//  thus cannot be copyrighted.  This software is freely available
//  to the public for use. The National Library of Medicine and the U.S.
//  Government have not placed any restriction on its use or reproduction.
//
//  Although all reasonable efforts have been taken to ensure the accuracy
//  and reliability of the software and data, the NLM and the U.S.
//  Government do not and cannot warrant the performance or results that
//  may be obtained by using this software or data. The NLM and the U.S.
//  Government disclaim all warranties, express or implied, including
//  warranties of performance, merchantability or fitness for any particular
//  purpose.
//
//  Please cite the author in any work or product based on this material.
//
// ===========================================================================
//

/// \file
/// \author  Bradley Lowekamp
/// \version $Revision: 1.6 $
/// \brief  
///
///


#include "FileUtilities.h"


#include <sys/types.h>
#include <sys/stat.h>

#if defined(WIN32)
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <stdexcept>
#include <stdlib.h>
#include <iostream> 

#define MAX_PATH 1024

namespace fileutil {

  
  PathList GetPathFromEnviroment(const std::string &env) {
    PathList pl;
    char *cstr =  getenv(env.c_str());
    if (!cstr)
      return pl;

    return GetPathListFromColonSeparatedList(cstr);
  }

  PathList GetPathListFromColonSeparatedList(const std::string &value) {
    PathList pl;

    if (value == "")
      return pl;
   
    std::string::size_type i = 0;
    
    while (i != std::string::npos) {
      std::string::size_type j = i;
      i = value.find(':', j);
      if (i == std::string::npos)
	pl.push_back(PathName(value.substr(j, value.length() - j)));
      else {
	pl.push_back(PathName(value.substr(j, i - j )));
	++i;
      }
    }

    return pl;
  }
  

  bool SetCurrentDirectory(const PathName &path) {
    if (!path.IsDirectory())
      throw std::runtime_error(std::string("\"") + path.GetNativePathName() + "\" is not a directory.");
    return chdir(path.GetNativePathName().c_str()) != -1;
  }

  PathName GetCurrentDirectory(void) {
    std::string result;
    struct stat s;

    if (!stat(".", &s)) {
      char currentName[MAX_PATH];
      getcwd(currentName, MAX_PATH);
      result = currentName;
    }
    return PathName(result);
  }
  
  
	
}

