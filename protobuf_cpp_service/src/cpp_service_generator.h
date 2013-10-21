/*
 * cpp_service_generator.h
 *
 *  Created on: 2013-3-7
 *      Author: rogeryang
 */

#ifndef CPP_SERVICE_GENERATOR_H_
#define CPP_SERVICE_GENERATOR_H_

#include <map>
#include <string>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/compiler/code_generator.h>

using namespace std;

namespace google {
namespace protobuf {
namespace compiler {

class CppServiceCodeGenerator : public CodeGenerator
{
public:
  CppServiceCodeGenerator(){}
  virtual ~CppServiceCodeGenerator(){}

  virtual bool Generate(const FileDescriptor* file,
                        const string& parameter,
                        GeneratorContext* generator_context,
                        string* error) const;

private:

bool GenerateSppMainCC(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* proto_file, string& err ) const;

bool GenerateBaseDefH(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* proto_file, string& err ) const;

bool GenerateServiceH(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* proto_file, string& err ) const;

bool GenerateServiceCC(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* proto_file, string& err ) const;

bool GenerateMakefile(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* proto_file, string& err ) const;

bool GenerateServiceApiH(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* proto_file, string& err ) const;

bool GenerateServiceApiCC(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* proto_file, string& err ) const;
};

}//namespace compiler
} //namespace protobuf
} //namespace google

#endif /* CPP_SERVICE_GENERATOR_H_ */
