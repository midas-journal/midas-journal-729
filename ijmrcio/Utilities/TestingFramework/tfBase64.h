#ifndef BASE64_H 	/* -*- c++ -*- */
#define BASE64_H


//  $Id: Base64.h,v 1.4 2005-06-30 14:10:52 dave Exp $
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
/// \brief  Implmentation of base 64 encoding
///
///

#include <string>


namespace testutil {

  /// Implements Base64 encoding and decoding as defined by RFC 2045: "Multipurpose Internet
  /// Mail Extensions (MIME) Part One: Format of Internet Message Bodies" page 23.
  /// More information about this class is available from <a target="_top" href=
  /// "http://ostermiller.org/utils/Base64.html">ostermiller.org</a>.
  ///
  /// <blockquote> 
  /// <p>The Base64 Content-Transfer-Encoding is designed to represent
  /// arbitrary sequences of octets in a form that need not be humanly
  /// readable.  The encoding and decoding algorithms are simple, but the
  /// encoded data are consistently only about 33 percent larger than the
  /// unencoded data.  This encoding is virtually identical to the one used
  /// in Privacy Enhanced Mail (PEM) applications, as defined in RFC 1421.</p>
  ///
  /// <p>A 65-character subset of US-ASCII is used, enabling 6 bits to be
  /// represented per printable character. (The extra 65th character, "=",
  /// is used to signify a special processing function.)</p>
  ///
  /// <p>NOTE:  This subset has the important property that it is represented
  /// identically in all versions of ISO 646, including US-ASCII, and all
  /// characters in the subset are also represented identically in all
  /// versions of EBCDIC. Other popular encodings, such as the encoding
  /// used by the uuencode utility, Macintosh binhex 4.0 [RFC-1741], and
  /// the base85 encoding specified as part of Level 2 PostScript, do not
  /// share these properties, and thus do not fulfill the portability
  /// requirements a binary transport encoding for mail must meet.</p>
  ///
  /// <p>The encoding process represents 24-bit groups of input bits as output
  /// strings of 4 encoded characters.  Proceeding from left to right, a
  /// 24-bit input group is formed by concatenating 3 8bit input groups.
  /// These 24 bits are then treated as 4 concatenated 6-bit groups, each
  /// of which is translated into a single digit in the base64 alphabet.
  /// When encoding a bit stream via the base64 encoding, the bit stream
  /// must be presumed to be ordered with the most-significant-bit first.
  /// That is, the first bit in the stream will be the high-order bit in
  /// the first 8bit byte, and the eighth bit will be the low-order bit in
  /// the first 8bit byte, and so on.</p>
  /// 
  /// <p>Each 6-bit group is used as an index into an array of 64 printable
  /// characters.  The character referenced by the index is placed in the
  /// output string.  These characters, identified in Table 1, below, are
  /// selected so as to be universally representable, and the set excludes
  /// characters with particular significance to SMTP (e.g., ".", CR, LF)
  /// and to the multipart boundary delimiters defined in RFC 2046 (e.g.,
  /// "-").</p>
  ///
  /// <pre>
  ///                     Table 1: The Base64 Alphabet
  ///
  ///      Value Encoding  Value Encoding  Value Encoding  Value Encoding
  ///          0 A            17 R            34 i            51 z
  ///          1 B            18 S            35 j            52 0
  ///          2 C            19 T            36 k            53 1
  ///          3 D            20 U            37 l            54 2
  ///          4 E            21 V            38 m            55 3
  ///          5 F            22 W            39 n            56 4
  ///          6 G            23 X            40 o            57 5
  ///          7 H            24 Y            41 p            58 6
  ///          8 I            25 Z            42 q            59 7
  ///          9 J            26 a            43 r            60 8
  ///         10 K            27 b            44 s            61 9
  ///         11 L            28 c            45 t            62 +
  ///         12 M            29 d            46 u            63 /
  ///         13 N            30 e            47 v
  ///         14 O            31 f            48 w         (pad) =
  ///         15 P            32 g            49 x
  ///         16 Q            33 h            50 y
  /// <\pre>
  ///
  /// <p>The encoded output stream must be represented in lines of no more
  /// than 76 characters each.  All line breaks or other characters not
  /// found in Table 1 must be ignored by decoding software.  In base64
  /// data, characters other than those in Table 1, line breaks, and other
  /// white space probably indicate a transmission error, about which a
  /// warning message or even a message rejection might be appropriate
  /// under some circumstances.</p>
  ///
  /// <p>Special processing is performed if fewer than 24 bits are available
  /// at the end of the data being encoded.  A full encoding quantum is
  /// always completed at the end of a body.  When fewer than 24 input bits
  /// are available in an input group, zero bits are added (on the right)
  /// to form an integral number of 6-bit groups.  Padding at the end of
  /// the data is performed using the "=" character.  Since all base64
  /// input is an integral number of octets, only the following cases can
  /// arise: (1) the final quantum of encoding input is an integral
  /// multiple of 24 bits; here, the final unit of encoded output will be
  /// an integral multiple of 4 characters with no "=" padding, (2) the
  /// final quantum of encoding input is exactly 8 bits; here, the final
  /// unit of encoded output will be two characters followed by two "="
  /// padding characters, or (3) the final quantum of encoding input is
  /// exactly 16 bits; here, the final unit of encoded output will be three
  /// characters followed by one "=" padding character.
  ///
  /// <p>Because it is used only for padding at the end of the data, the
  /// occurrence of any "=" characters may be taken as evidence that the
  /// end of the data has been reached (without truncation in transit).  No
  /// such assurance is possible, however, when the number of octets
  /// transmitted was a multiple of three and no "=" characters are
  /// present.</p?
  ///
  /// <p>Any characters outside of the base64 alphabet are to be ignored in
  /// base64-encoded data.</p?
  ///
  /// <p>Care must be taken to use the proper octets for line breaks if base64
  /// encoding is applied directly to text material that has not been
  /// converted to canonical form.  In particular, text line breaks must be
  /// converted into CRLF sequences prior to base64 encoding.  The
  /// important thing to note is that this may be done directly by the
  /// encoder rather than in a prior canonicalization step in some
  /// implementations.</p?
  ///
  /// <p>NOTE: There is no need to worry about quoting potential boundary
  /// delimiters within base64-encoded bodies within multipart entities
  /// because no hyphen characters are used in the base64
  /// encoding.</p>
  /// </blockquote>
  ///
  class Base64 {
   
  public:

    Base64(void);

    //@{
    /// \brief Set/Get/Toggle Adding required new lines to endcoding
    ///
    /// The spec indicates that no line shall be longer that 72
    /// charactors. The new line charactor will not be added at the
    /// end of the encoding.
    ///
    /// Default value is true.
    bool GetAddNewLines(void) const;
    void SetAddNewLines(const bool b);
    void NewLinesOn(void);
    void NeLinesOff(void);
    //@}

    //@{
    /// \brief Set/Get/Toggle Ignoring invalid charactors
    ///
    /// The spec suggests that any invalid charactors indicate of
    /// possible transmittion error. If this value is false to
    /// action is taken, other wise an exception is thrown.
    ///
    /// Default value is false.
    bool GetIgnoreInvalids(void) const;
    void SetIgnoreInvalids(const bool b);
    void IgnoreInvalidsOn(void);
    void IgnoreInvalidsOff(void);
    //@}

    /// \brief Encodes data into base 64
    ///
    /// \param _input the data to be encoded
    /// \param numBytes is the size of the input
    /// \return a base64 encoding of the input
    ///
    /// \see SetAddNewLines affects the output
    std::string Encode(const void *_input, size_t numBytes);	
      
    /// \brief Checks to see if a string conforms to base64
    ///
    /// \param input the base64 encoding to be verified
    /// \return 0 if input is not a valid base64 encodeing,
    /// otherwise the number of bytes that will be decoded
    ///
    /// \note the max of 72 chars per line is not verified
    size_t IsBase64(const std::string &input);
      
    /// \brief Decode base64 into data
    ///
    /// \param input the base64 encoding to be decoded
    /// \param _output the pre-allocate data to 
    /// \return the number of bytes that were decoded into output
    ///
    /// if there is a error, an excpetion will be thrown
    ///
    /// all of input will not be decoded if padding is reached
    /// before the end of the string
    ///
    /// \note the max of 72 chars per line is not verified
    size_t Decode(const std::string &input, void *_output);
      
  protected:
      
    /// table to convert 6-bits to base 64
    static const char base64chars[64];
      
    // the bit value of a non-base64 char
    static const unsigned char InvalidBase64Bits = 255;

    // the bit value of a whitespace
    static const unsigned char WhiteSpaceBits = 254;

    // the bit value of the padding char
    static const unsigned char PaddingBits = 64;
      
      
      
    /// \brief table to convert base64 to 6 bits
    ///
    /// 255 - not code
    /// 254 - white space
    /// 64 - special padding
    /// base64bits[base64chars[v]] == v
    static const unsigned char base64bits[256];

    bool addNewLines;
    bool ignoreInvalids;
    

  };
}


#endif // BASE64_H
