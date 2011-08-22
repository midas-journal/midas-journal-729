//  $Id: PathName.cpp,v 1.11 2006-02-16 18:05:25 blowek1 Exp $
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
/// \version $Revision: 1.11 $
/// \brief  
///
///


#include "PathName.h"


#include <sys/types.h>
#include <sys/stat.h>

#if defined(WIN32)
#include <io.h>
#include <direct.h>
#define X_OK 0
#define W_OK 2
#define R_OK 4
#else
#include <unistd.h>
#include <dirent.h>
#endif

#include <stdexcept>
#include <stdlib.h>

#define MAX_PATH 1024

namespace fileutil {


  PathName::PathName(void) {
  }

  
  PathName::~PathName(void) {
  }
  
  PathName::PathName(const std::string &path) : pathName(path), statCached(false) {
  }

  PathName::PathName(const char *path)  : pathName(path), statCached(false) {
  }

  PathName::PathName(const PathName &f) : pathName(f.pathName), statCached(false) {
  }
  
  PathName &PathName::operator=(const PathName &f) {
    this->pathName = f.pathName;
    this->InvalidateFileStatCache();
    return *this;
  }


  
  void PathName::SetPathName(const std::string &path) {
    this->pathName = path;
    this->InvalidateFileStatCache();
    
  }
  const std::string &PathName::GetPathName(void) const {
    return this->pathName;
  }

  
  std::string PathName::GetNativePathName(void) const { 
#if defined(WIN32) // windows
    // replace all / with \ . 
    //
    std::string::size_type i = 0;
    std::string nativePathName(this->GetPathName());
    while ((i = nativePathName.find('/', i)) != std::string::npos) {
      nativePathName[i] = '\\';
    }
    return nativePathName;
    
#else /// POSIX UNIX
    return this->GetPathName();
#endif
  }

  bool PathName::IsNativeFormat(void) const {
#if defined(WIN32) // windows
    if (this->GetPathName().size() > 2) {
      // absolute path with drive
      if (this->pathName[1] == ':' && isalpha(this->pathName[0]))
	return true;
      // absolute path with network drive
      else if (this->pathName[1] == '\\' || this->pathName[1] == '/' &&
	       this->pathName[0] == '\\'|| this->pathName[0] == '/' )
	return true;
    }
    return false;
    
#else /// POSIX UNIX
    return true;
#endif
  }

  
  PathName &PathName::Append(const PathName &f) {
    if (f.GetPathType() == Absolute || this->GetPathName().length() == 0)
      this->SetPathName(f.GetPathName());
    else {
#if defined(WIN32) // windows
      // remove trailing '\\'
      std::string path = this->GetNativePathName();
      std::string::size_type i = path.length() - 1;
      while (i && path[i] == '\\')
	--i;
      path.erase(i + 1, path.length() - i -1);
      this->SetPathName(path + '\\' + f.GetNativePathName());      
    
#else /// POSIX UNIX
      // remove trailing '/'
      std::string path = this->GetNativePathName();
      std::string::size_type i = path.length() - 1;
      while (i && path[i] == '/')
	--i;
      path.erase(i + 1, path.length() - i - 1);
      this->SetPathName(path + '/' + f.GetNativePathName());      
    
#endif
    }
    return *this;
  }

  
  PathName::PathType PathName::GetPathType(void) const {
    if (this->GetPathName().length() == 0) {
      return Relative;
    }
#if defined(WIN32) // windows
    std::string path = this->GetNativePathName();
    //  local drive specified
    if (path.size() > 3  && 
	isalpha(path[0]) &&
	path[1] == ':') {
      if (path[2] == '\\')
	return Absolute;
      else 
	return DriveRelative;
    }
    // network dirve  (UNC)  
    if (path.size() > 2 && 
	path[0] == '\\' &&
	path[1] == '\\')
      return Absolute;
    
    return Relative;
        
    
#else /// POSIX UNIX
    if (this->GetPathName()[0] == '/')
      return Absolute;
    else
      return Relative;
#endif
  }

  PathName PathName::DirectoryName(void) const {
    std::string path = this->GetNativePathName();
    std::string::size_type i;
#if defined(WIN32) // windows
    i = path.rfind('\\');
    if (i == 1 && path[0] == '\\')
      i = std::string::npos;
#else /// POSIX UNIX
    i = path.rfind('/');
#endif
    if (i == 0 || i == std::string::npos)
      return PathName(".");
    return PathName(path.substr(0, i));
  }


  PathName PathName::Tail(void) const {
    
    std::string path = this->GetNativePathName();
    std::string::size_type i;
    if (path.length() == 0)
      return PathName("");

#if defined(WIN32) // windows
    i = path.rfind('\\', path.length() - 1);
#else /// POSIX UNIX
    i = path.rfind('/', path.length() - 1);
#endif
    if (i == std::string::npos)
      return PathName(path);
    else
      return PathName(path.substr(i+1, path.length() - i - 1));
  }

  
  std::string PathName::Extension(void) const {
    std::string tail = this->Tail().GetNativePathName();
    std::string::size_type i = tail.rfind('.');
    if (i == std::string::npos)
      return std::string("");
    else 
      return tail.substr(i+1, tail.length() - i - 1);    
  }

  bool PathName::IsWritable(void) const {    
    int mode = W_OK;
    if (this->IsDirectory())
      mode |= X_OK;
    return !access(this->GetNativePathName().c_str(), mode);
  }

  bool PathName::IsReadable(void) const {    
    int mode = R_OK;
    if (this->IsDirectory())
      mode |= X_OK;
    return !access(this->GetNativePathName().c_str(), mode);
  }

  
  bool PathName::IsExecutable(void) const { 
    int mode = X_OK;
    return !access(this->GetNativePathName().c_str(), mode);
  }

  bool PathName::IsFile(void) const {
    struct stat s;


    if (!stat(this->GetNativePathName().c_str(), &s)) 		
#if defined(WIN32)
      return (S_IFREG & s.st_mode)!= 0;
#else
      return S_ISREG(s.st_mode);
#endif
    return false;
  }

  
  bool PathName::IsDirectory(void) const {
    struct stat s;

     if (!stat(this->GetNativePathName().c_str(), &s))       
#if defined(WIN32)
       return (S_IFDIR & s.st_mode) != 0;
#else
       return S_ISDIR(s.st_mode);
#endif
    return false;
  }

  bool PathName::Exists(void) const {
    struct stat s;
    return !stat(this->GetNativePathName().c_str(), &s);
  }



  
  PathName PathName::SearchPathList(const PathList &pathList) const {
    PathName fullFileName;
    
    for (PathList::const_iterator i = pathList.begin(); i != pathList.end(); ++i) {
      
      fullFileName = *i;
      fullFileName.Append(*this);
      if (fullFileName.IsFile())
	return fullFileName;
    }
    return fullFileName;
  }


  
  PathList PathName::SearchDirectory(void) const {
    PathList opl;
    if (!this->IsDirectory())
      return opl;
  
#if defined(WIN32)
#error This needs to be implemented
#else
    DIR *dir;
    struct dirent *dirp;
    if ((dir = opendir(this->GetNativePathName().c_str())) != NULL) {
      
      while ((dirp = readdir(dir)) != NULL) {
	opl.push_back(PathName(dirp->d_name));
      }

      closedir(dir);
    }

#endif
    return opl;
    
  }


  
  void PathName::InvalidateFileStatCache(void) {
    this->statCached=false;
  }
   PathName::FileStat &PathName::GetFileStatCache(void) {
    return this->fileStat;
  }
    
  
  
  
	
}

