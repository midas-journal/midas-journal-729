#ifndef FILEUTILITIES_PATHNAME_H 	/* -*- c++ -*- */
#define FILEUTILITIES_PATHNAME_H


//  $Id: PathName.h,v 1.7 2006-02-14 16:28:36 blowek1 Exp $
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
/// \version $Revision: 1.7 $
/// \brief  
///
///

#include <string>
#include <ostream>
#include <list>
#include "FileUtilities/FileUtilitiesConfigure.h"

namespace fileutil {

  class PathName;

  /// \brief a handy definition for they type used for paths.
  ///
  /// Each string in the path is a absolute or relative path name. 
  typedef std::list<PathName> PathList;

  /// a component is
  /// a paht name is
  class FILEUTILITIES_EXPORT PathName {
  public:
    
    PathName(void);
    virtual ~PathName(void);
    PathName(const std::string &path);
    PathName(const char *path);
    PathName(const PathName &f);

    PathName &operator=(const PathName &f);


    virtual void SetPathName(const std::string &path);
    virtual const std::string &GetPathName(void) const;
    virtual std::string GetNativePathName(void) const;


    
    virtual bool IsNativeFormat(void) const;

    /// \brief joins the current path and f with a directory seporator
    ///
    /// returns a reference to this
    ///
    /// \note if f is an absolute path name then it overides any
    /// current value
    virtual PathName &Append(const PathName &f);

    enum PathType {Relative, Absolute, DriveRelative};

    
    virtual PathType GetPathType(void) const;
    
    /// \brief Returns all but the last componet of the path
    ///
    /// If the path contains only a single component then "." is
    /// returned. 
    /// Example:
    /// \code
    /// PathName("/a/b/c").DirectoryName();
    /// \endcode    
    /// will return "/a/b"
    virtual PathName DirectoryName(void) const;

 

    /// \brief Returns the last component
    ///
    /// Complementary to DirectoryName(). 
    ///
    /// \note this may not nessecarily be an actual file or a directory.
    virtual PathName Tail(void) const;

    /// \brief Return every things after the last period
    virtual std::string Extension(void) const;

    
    /// \brief check to see if the directory or file is writable
    virtual bool IsWritable(void) const;
    
    /// \brief checks to see if the directory or file is readable
    virtual bool IsReadable(void) const;

    /// \brief checks to see if the directory or file is readable
    virtual bool IsExecutable(void) const;
    
    /// \brief checks to see if fileName exists (visible to the user) as
    /// a file
    ///
    /// What it the behavior if fileName is a symbolic link or device?
    virtual bool IsFile(void) const;
    
    /// \brief checks to see if fileName exists (visible to the user) as
    /// a directory
    virtual bool IsDirectory(void) const;
    

    virtual bool Exists(void) const;


    /// \brief Searches a path list for a file
    ///
    /// searches the path list for the currently set path as a file in
    /// the path list. The first path in the path list sequence, has
    /// this path appended to the end.
    ///
    /// \param pathList a list of paths. Relative
    /// paths are seached from the current directory
    /// \return the absolute or relative path of the file. If this path file
    /// was not found as a regular file then "" will be returned.
    ///
    /// \note this does not include searching the currect directory
    PathName SearchPathList(const PathList &pathList) const;
    
    
    // \brief Returns a list of file in this directory
    //
    // IsDirectory() is true or an empty list is returned
    //
    // Returns a list (in no particular order) of files in the
    // directory named by this object. The PathNames are relative to
    // this PathName.
    PathList SearchDirectory(void) const;
    
    
  protected:
  
    typedef int FileStat;


    virtual void InvalidateFileStatCache(void);
    virtual FileStat &GetFileStatCache(void);
    
  private:

    std::string pathName;
    FileStat fileStat;
    bool statCached;
  };

 
}


#endif // FILEUTILITIES_PATHNAME_H
