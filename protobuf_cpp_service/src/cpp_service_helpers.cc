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

#include <stdio.h>
#include <limits>
#include <vector>
#include "cpp_service_helpers.h"


namespace google {
namespace protobuf {
namespace compiler {
namespace cpp_service {


string StripProto(const string& filename) {
  if (HasSuffixString(filename, ".protodevel")) {
    return StripSuffixString(filename, ".protodevel");
  } else {
    return StripSuffixString(filename, ".proto");
  }
}

uint32_t StringReplace( std::string& ss, const std::string& src,
                        const std::string& dst )
{
    uint32_t replace_count = 0;
    std::string::size_type pos = 0;
    std::string::size_type srclen = src.size();
    std::string::size_type dstlen = dst.size();
    while( (pos=ss.find(src, pos)) != std::string::npos)
    {
        ++replace_count;
        ss.replace(pos, srclen, dst);
        pos += dstlen;
    }
    return replace_count;
}

void StringToUpper( std::string& ss )
{
    transform ( ss.begin(),ss.end(), ss.begin(),
                ::toupper );
}

void StringSplit( const std::string& str,
                  const std::string& delim,
                  std::vector<std::string>& items )
{
    items.clear();

    std::string sElement;
    std::string::size_type bpos = 0;
    std::string::size_type epos = 0;
    std::string::size_type nlen = delim.size();
    while ((epos=str.find(delim, epos)) != std::string::npos)
    {
        sElement = str.substr(bpos, epos-bpos);
        //非空的字符串才返回
        if(!sElement.empty())
        {
            items.push_back(sElement);
        }
        epos += nlen;
        bpos = epos;
    }
    //结尾如果还有，需要返回
    sElement = str.substr(bpos, str.size()-bpos);
    if(!sElement.empty())
    {
        items.push_back(sElement);
    }
}

string ToClassName( string full_name )
{
    cpp_service::StringReplace( full_name, ".", "::" );
    return full_name;
}

string ToFileName( string full_name )
{
    cpp_service::StringReplace( full_name, ".", "_" );
    return full_name;
}

string GetHMacroName( string file_name )
{
    cpp_service::StringReplace( file_name, ".", "_" );
    cpp_service::StringToUpper( file_name );
    return file_name;
}

vector<string> GetNameSpace( const FileDescriptor* file )
{
    vector<string> ret;
    StringSplit( file->package(), ".", ret );
    return ret;
}

string GetMethodNamespace( const MethodDescriptor* method )
{
    return ToClassName( method->service()->file()->package() );
}

//get the cmd of one method
string GetMethodCmd( const MethodDescriptor* method )
{
    string name_space = GetMethodNamespace( method );
    if( name_space.size() > 0 )
    {
        return name_space + "::" + GetMethodCmdNoNS(method);
    }
    else
    {
        return GetMethodCmdNoNS(method);
    }
}

string GetMethodReqName( const MethodDescriptor* method )
{
    return "cReq_" + method->name();
}

string GetMethodRespName( const MethodDescriptor* method )
{
    return "cResp_" + method->name();
}

string GetMethodCmdNoNS( const MethodDescriptor* method )
{
    string method_name = method->name();
    StringToUpper( method_name );
    return method_name + "_CMD";
}

string GetMethodInClass( const MethodDescriptor* method )
{
    return ToClassName( method->input_type()->full_name() );
}

string GetMethodOutClass( const MethodDescriptor* method )
{
    return ToClassName( method->output_type()->full_name() );
}

string GetMethodDeclare( const MethodDescriptor* method )
{
    return string("int32_t ") + method->name() + "( const " +
           GetMethodInClass(method) + "& req,\n" +
           "    " + GetMethodOutClass(method) + "& resp )";
}

string GetMethodImp( const MethodDescriptor* method )
{
    return string("int32_t ") + method->service()->name() + "::" +
           method->name() + "( const " +
           GetMethodInClass(method) + "& req,\n" +
           "    " + GetMethodOutClass(method) + "& resp )\n" +
           "    {\n" +
           "        return 0;\n"
           "    }\n";
}

string GetServiceApiClassName( const ServiceDescriptor* service )
{
    return service->name() + "Api";
}

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
