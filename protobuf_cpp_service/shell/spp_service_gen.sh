#!/bin/sh

#use protoc gen code for spp
_protoc=/usr/local/services/protobuf-2.5.0/bin/protoc
_protoc_cpp_plugin=/usr/local/services/vrpc_gen-2.3/bin/cpp_service
_pkg_dir=/usr/local/services/vrpc_gen-2.3/pkg_resource/

if [ $# -ne 4 ]; then
    echo "usage: $0 proto_file_path proto_file proj_name output_dir"
    exit 1
fi

_proto_file_path=$1
_proto_file=$2
_proj_name=$3
_output_dir=$4

export LD_LIBRARY_PATH=/usr/local/services/protobuf-2.5.0/lib/:$LD_LIBRARY_PATH

#make proj dir
mkdir -p $_output_dir/$_proj_name
cd $_output_dir/$_proj_name
mkdir src
mkdir include
mkdir lib
mkdir conf
mkdir proto
cp $_proto_file proto/
cp -rf $_pkg_dir/cpp/include/* include/
cp -rf $_pkg_dir/cpp/lib/* lib/
mkdir -p release/api
mkdir -p release/api/lib
mkdir -p release/api/include/vrpc/base
mkdir -p release/spp_so

cd src
cp -rf $_pkg_dir/cpp/src/* ./

#gen cpp code
$_protoc --cpp_out=./ -I$_proto_file_path $_proto_file
if [ $? -ne 0 ]; then
	echo "protoc gen cpp exec error"
	exit 1
fi

#gen cpp_service code
$_protoc --plugin=protoc-gen-cpp_service=$_protoc_cpp_plugin --cpp_service_out=./ -I$_proto_file_path $_proto_file
if [ $? -ne 0 ]; then
	echo "protoc gen cpp_service exec error"
	exit 2
fi

#api release
cd ../
cp src/*_api.h release/api/include/
cp src/*.pb.h release/api/include/
cp src/vrpc/base/CApiBase.h release/api/include/vrpc/base/
cp src/vrpc/base/CObject.h release/api/include/vrpc/base/

########################################################################
#gen php code 
_pkg_dir_php=/usr/local/services/vrpc_gen-2.3/pkg_resource/php/

cd $_output_dir/$_proj_name
mkdir php
mkdir php/lib
mkdir php/message
mkdir php/proto

cp $_proto_file php/proto
cp -rf $_pkg_dir_php/lib/* php/lib/
cp -rf $_pkg_dir_php/message/* php/message/


#gen php & service code
cd $_pkg_dir_php/parser
/usr/local/services/php-5.2.14/bin/php -f magic.php $_proto_file $_output_dir/$_proj_name/php/ $_pkg_dir_php
if [ $? -ne 0 ]; then
	echo "protoc gen php exec error"
	exit 1
fi
########################################################################