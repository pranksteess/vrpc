// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#ifndef GOOGLE_PROTOBUF_COMPILER_CPP_HELPERS_H__
#define GOOGLE_PROTOBUF_COMPILER_CPP_HELPERS_H__

#include <string>
#include <vector>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace cpp_service {

// ----------------------------------------------------------------------
// ascii_isalnum()
//    Check if an ASCII character is alphanumeric.  We can't use ctype's
//    isalnum() because it is affected by locale.  This function is applied
//    to identifiers in the protocol buffer language, not to natural-language
//    strings, so locale should not be taken into account.
// ascii_isdigit()
//    Like above, but only accepts digits.
// ----------------------------------------------------------------------

inline bool ascii_isalnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9');
}

inline bool ascii_isdigit(char c) {
  return ('0' <= c && c <= '9');
}

// ----------------------------------------------------------------------
// HasPrefixString()
//    Check if a string begins with a given prefix.
// StripPrefixString()
//    Given a string and a putative prefix, returns the string minus the
//    prefix string if the prefix matches, otherwise the original
//    string.
// ----------------------------------------------------------------------
inline bool HasPrefixString(const string& str,
                            const string& prefix) {
  return str.size() >= prefix.size() &&
         str.compare(0, prefix.size(), prefix) == 0;
}

inline string StripPrefixString(const string& str, const string& prefix) {
  if (HasPrefixString(str, prefix)) {
    return str.substr(prefix.size());
  } else {
    return str;
  }
}

// ----------------------------------------------------------------------
// HasSuffixString()
//    Return true if str ends in suffix.
// StripSuffixString()
//    Given a string and a putative suffix, returns the string minus the
//    suffix string if the suffix matches, otherwise the original
//    string.
// ----------------------------------------------------------------------
inline bool HasSuffixString(const string& str,
                            const string& suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline string StripSuffixString(const string& str, const string& suffix) {
  if (HasSuffixString(str, suffix)) {
    return str.substr(0, str.size() - suffix.size());
  } else {
    return str;
  }
}

// Strips ".proto" or ".protodevel" from the end of a filename.
string StripProto(const string& filename);

// Do message classes in this file keep track of unknown fields?
inline bool HasUnknownFields(const FileDescriptor *file) {
  return file->options().optimize_for() != FileOptions::LITE_RUNTIME;
}

// Does this file have generated parsing, serialization, and other
// standard methods for which reflection-based fallback implementations exist?
inline bool HasGeneratedMethods(const FileDescriptor *file) {
  return file->options().optimize_for() != FileOptions::CODE_SIZE;
}

// Do message classes in this file have descriptor and refelction methods?
inline bool HasDescriptorMethods(const FileDescriptor *file) {
  return file->options().optimize_for() != FileOptions::LITE_RUNTIME;
}

// Should we generate generic services for this file?
inline bool HasGenericServices(const FileDescriptor *file) {
  return file->service_count() > 0 &&
         file->options().optimize_for() != FileOptions::LITE_RUNTIME;
}

// Should string fields in this file verify that their contents are UTF-8?
inline bool HasUtf8Verification(const FileDescriptor* file) {
  return file->options().optimize_for() != FileOptions::LITE_RUNTIME;
}

// Should we generate a separate, super-optimized code path for serializing to
// flat arrays?  We don't do this in Lite mode because we'd rather reduce code
// size.
inline bool HasFastArraySerialization(const FileDescriptor* file) {
  return file->options().optimize_for() == FileOptions::SPEED;
}

uint32_t StringReplace( std::string& ss, const std::string& src,
                        const std::string& dst );
void StringToUpper( std::string& ss );
string ToClassName( string full_name );
string ToFileName( string full_name );
string GetMethodNamespace( const MethodDescriptor* method );
string GetMethodCmd( const MethodDescriptor* method );
string GetMethodCmdNoNS( const MethodDescriptor* method );
string GetMethodInClass( const MethodDescriptor* method );
string GetMethodOutClass( const MethodDescriptor* method );
string GetMethodReqName( const MethodDescriptor* method );
string GetMethodRespName( const MethodDescriptor* method );
string GetHMacroName( string file_name );
vector<string> GetNameSpace( const FileDescriptor* file );
string GetMethodDeclare( const MethodDescriptor* method );
string GetMethodImp( const MethodDescriptor* method );
string GetServiceApiClassName( const ServiceDescriptor* service );

}  // namespace cpp_service
}  // namespace compiler
}  // namespace protobuf

}  // namespace google
#endif  // GOOGLE_PROTOBUF_COMPILER_CPP_HELPERS_H__
