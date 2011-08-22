#ifndef TESTUTIL_DIFF_H 	/* -*- c++ -*- */
#define TESTUTIL_DIFF_H


//  $Id: diff.h,v 1.3 2005-06-30 14:20:11 dave Exp $
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
/// \version $Revision: 1.3 $
/// \brief  
///
///

#include <string>
#include <ostream>

namespace testutil {

  /// \brief Prints the line by line difference of two strings to an output
  ///
  /// The two strings are compared line by line. If they match then no
  /// output is produced. Otherwise a sequence of change commands are
  /// produced of the following format:
  ///
  /// COMMAND
  /// < FROM-STRING-LINE
  /// < FROM-STRING-LINES...
  /// ---
  /// > TO-STRING-LINE
  /// > TO-STRING-LINES...
  ///
  /// There are three kinds of change commands. Each command consists
  /// of a line range, a letter, and then another line range. A line
  /// range can be a single line or a comma separated range of
  /// lines. A line number is the 0 based index into the respective
  /// original string. The command formats are:
  /// 
  ///  `LaR'
  ///       Add the lines in range R of the 'to' string after line L of the
  ///       'from' string.   
  ///
  ///  `FcT'
  ///       Replace the lines in range F of the 'to' string with lines in range
  ///       T of the 'from' string.  This is like a combined add and delete, but
  ///       more compact.  F
  ///
  ///  `RdL'
  ///       Delete the lines in range R from the 'to' string; line L is where
  ///       they would have appeared in the 'from' string had they not been
  ///       deleted. 
  ///
  void Diff(const std::string &from, const std::string &to, std::ostream &os);
      
	
}


//
// ===========================================================================
//
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/05/24 20:31:35  blowek1
// added documentation
//
// Revision 1.1  2003/12/12 20:21:03  blowek1
// move file utils to a separate directory
//
// Revision 1.3  2003/12/04 20:17:49  blowek1
// added documentation
//
// Revision 1.2  2003/12/04 20:00:55  blowek1
// added file function
//
// Revision 1.1  2003/12/03 16:17:07  blowek1
// incremental check in
//
//
//
//
// ===========================================================================
//

#endif // TESTUTIL_MISC_H
