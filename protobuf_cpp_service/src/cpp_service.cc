/*
 * cpp_service.cc
 *
 *  Created on: 2013-3-7
 *      Author: rogeryang
 */
#include <google/protobuf/compiler/plugin.h>
#include "cpp_service_generator.h"

int main( int argc, char** argv )
{
  google::protobuf::compiler::CppServiceCodeGenerator generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}



