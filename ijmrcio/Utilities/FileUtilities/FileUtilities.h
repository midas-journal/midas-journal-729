#ifndef FILEUTILITIES_H 	/* -*- c++ -*- */
#define FILEUTILITIES_H


//  $Id: FileUtil.h,v 1.5 2005-06-17 21:02:59 blowek1 Exp $
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
/// \version $Revision: 1.5 $
/// \brief  
///
///

#include <string>
#include "PathName.h"
#include "FileUtilitiesConfigure.h"

namespace fileutil {

 
  /// \brief get a path list from the enviroment
  ///
  /// env is the name of the enviroment variable with the value of a
  /// colon separated sequence of path (relative or absolute.
  FILEUTILITIES_EXPORT PathList GetPathFromEnviroment(const std::string &env);

  /// \brief extract a path list from a color separated list
  FILEUTILITIES_EXPORT PathList GetPathListFromColonSeparatedList(const std::string &value);



  /// \brief changes the current working directory to path, if
  /// !IsDirectory() then an exception will be thrown
  FILEUTILITIES_EXPORT bool SetCurrentDirectory(const PathName &path);

#ifdef GetCurrentDirectory
#undef GetCurrentDirectory
#endif 
  /// \brief returns the current working directory
  FILEUTILITIES_EXPORT PathName GetCurrentDirectory(void);
  
}

#endif // FILEUTILITIES_H
