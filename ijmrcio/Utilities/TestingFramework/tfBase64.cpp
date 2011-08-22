//  $Id: Base64.cpp,v 1.4 2005-07-01 13:38:44 blowek1 Exp $
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
/// \version $Revision: 1.4 $
/// \brief  
///
///

#include "tfBase64.h"
#include <string>
#include <exception>
#include <stdexcept>
#include <new>
#include <limits>
#include <assert.h>



namespace testutil {
  
    
  Base64::Base64(void)
    : addNewLines(true), ignoreInvalids(false) {
  }

    
  bool Base64::GetAddNewLines(void) const {
    return this->addNewLines;
  }

  void Base64::SetAddNewLines(const bool b) {
    this->addNewLines = b;
  }

  void Base64::NewLinesOn(void) {
    this->addNewLines = true;
  }

  void Base64::NeLinesOff(void) {
    this->addNewLines = false;
  }

    
  bool Base64::GetIgnoreInvalids(void) const {
    return this->ignoreInvalids;
  }

  void Base64::SetIgnoreInvalids(const bool b) {
    this->ignoreInvalids = b;
  }

  void Base64::IgnoreInvalidsOn(void) {
    this->ignoreInvalids = true;
  }

  void Base64::IgnoreInvalidsOff(void) {
    this->ignoreInvalids = false;
  }

  std::string Base64::Encode(const void *_input, size_t numBytes) {
    std::string::size_type numChars = ((numBytes+2)/3)*4;
    if (this->addNewLines)	  
      numChars += numChars/72;
    std::string output(numChars, '\0');
    const unsigned char *input = static_cast<const unsigned char *>(_input);
	
	
    // The basic idea is that the three bytes get split into
    // four bytes along these lines:
    //      [AAAAAABB] [BBBBCCCC] [CCDDDDDD]
    // [xxAAAAAA] [xxBBBBBB] [xxCCCCCC] [xxDDDDDD]
    // bytes are considered to be zero when absent.
    // the four bytes are then mapped to common ASCII symbols
    size_t i = 0; // index into the output base64
    size_t j = 0; // index into the input
	
      
    while(numBytes >= 3) {
      output[i++] = base64chars[(input[j] >> 2)];
      output[i++] = base64chars[((input[j] << 4) & 0x30) | (input[j+1] >> 4)];
      output[i++] = base64chars[((input[j+1] << 2) & 0x3c) | (input[j+2] >> 6)];
      output[i++] = base64chars[(input[j+2] & 0x3f)];
      j += 3;
	  
      if (this->addNewLines && !(i%72))
	output[i++] = '\n';
	  
      numBytes -= 3;
    }
	
    // pad the output
    if (numBytes == 2) {	
      output[i++] = base64chars[(input[j] >> 2)];
      output[i++] = base64chars[((input[j] << 4) & 0x30) | (input[j+1] >> 4)];
      output[i++] = base64chars[((input[j+1] << 2) & 0x3c)];
      output[i++] = '=';
    } else if (numBytes == 1) {	
      output[i++] = base64chars[(input[j] >> 2)];
      output[i++] = base64chars[((input[j] << 4) & 0x30)];
      output[i++] = '=';
      output[i++] = '=';
    } 
	
    return output;
  }
      
  size_t Base64::IsBase64(const std::string &input) {
    size_t i = 0;     // index into encode input
    int byteNumber = 0;  // 0,1,3 indicates how to decode
    size_t bytesRead = 0;
    char byte;    
    bool padded = false;

    while (i < input.length()) {	  
	  
      byte = input[i++];
      if (base64bits[ptrdiff_t(byte)] <= 64) {
	    
	if (base64bits[ptrdiff_t(byte)] == PaddingBits) {
	  padded = true;	      
	  byteNumber++;
	} else if (padded) {
	  // the input was improperly terminated
	  return 0;
	} else {	    
	  // have a byte with data..
	  // look up in inverse table to get the bits
	  byteNumber++;
	  bytesRead++;
	}

	if (byteNumber == 4){
	  byteNumber = 0;
	  if (padded) 
	    break;
	}
	    

      } else if (base64bits[ptrdiff_t(byte)] == WhiteSpaceBits) {
	// white space, just ignore
      } else if (base64bits[ptrdiff_t(byte)] == InvalidBase64Bits) {
	if (!this->ignoreInvalids)
	  return 0;
      } else {
	return 0;
      }
    }

    if (byteNumber != 0)
      return 0;

    return bytesRead;
  }
      
  size_t Base64::Decode(const std::string &input, void *_output) {
	
    unsigned char *output = static_cast<unsigned char *>(_output);
    size_t i = 0; // index into encode input
    size_t j = 0; // index to output
    int byteNumber = 0; // 0,1,3 indicates how to decode
    size_t bytesRead = 0;
    char buffer[4];
    char byte;
    bool padded = false;
	

    while (i < input.length()) {	  
	  
      byte = input[i++];
      if (base64bits[ptrdiff_t(byte)] <= 64) {
	    
	if (base64bits[ptrdiff_t(byte)] == PaddingBits) {
	  if (!padded) {
	    // first pad decode what we have
	    padded = true;	 
	    if (byteNumber == 2) {
	      output[j++] = buffer[0] << 2 | buffer[1] >> 4;
	      output[j++] = buffer[1] << 4;
	    } else if (byteNumber == 3) {
	      output[j++] = buffer[0] << 2 | buffer[1] >> 4;
	      output[j++] = buffer[1] << 4 | buffer[2] >> 2;
	      output[j++] = buffer[2] << 6;
	    } else {
	      std::runtime_error("invalid base64 string, improper termination");
	    }
	  }
	  byteNumber++;
	      
	  // padding finishedm we are done
	  if (byteNumber == 4)
	    break;

	} else if (padded) {
	  // the input was improperly terminated
	  throw std::runtime_error("invalid base64 string, improper termination");
	} else {	    
	  // have a byte with data..
	  // look up in inverse table to get the bits
	  buffer[byteNumber++] = base64bits[ptrdiff_t(byte)];	      
	  bytesRead++;

	  if (byteNumber == 4){
	    // The basic idea is that the four bytes will get reconstituted
	    // into three bytes along these lines:
	    // [xxAAAAAA] [xxBBBBBB] [xxCCCCCC] [xxDDDDDD]
	    //      [AAAAAABB] [BBBBCCCC] [CCDDDDDD]
	    // bytes are considered to be zero when absent.
	    output[j++] = buffer[0] << 2 | buffer[1] >> 4;
	    output[j++] = buffer[1] << 4 | buffer[2] >> 2;
	    output[j++] = buffer[2] << 6 | buffer[3];
	    byteNumber = 0;
	    bytesRead--;
	  }
	      
	}
	     
      } else if (base64bits[ptrdiff_t(byte)] == WhiteSpaceBits) {
	// white space, just ignore
      } else if (base64bits[ptrdiff_t(byte)] == InvalidBase64Bits) {
	if (!this->ignoreInvalids)
	  throw std::runtime_error("invalid base64 string, invalid char");
      } else {
	throw std::runtime_error("corrupted base64bits");
      }
    }

    // make sure the out put was the correct number of whole parts
    if (byteNumber != 0)
      throw std::runtime_error("invalid base64 string, unexpected end");
	  

    return bytesRead;
  }


  
  // table to convert 6-bits to base 64
  const char Base64::base64chars[64] = {
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/',
  };
      
      
  // table to convert base64 to 6 bits
  const unsigned char Base64::base64bits[256] = {
    255,255,255,255,255,255,255,255,254,254,254,254,254,254,255,255, // 16
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, // 32     
    255,255,255,255,255,255,255,255,255,255,255, 62,255,255,255, 63, // 48
     52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255, 64,255,255, // 64
    255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, // 80
     15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255, // 96     
    255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, // 112
     41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,255,255,255,255,255, // 128
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, // 144
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, // 160     
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, // 176
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, // 192
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, // 208
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, // 224     
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, // 240
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255  // 256
  };

}
