/*
 * cpp_service_generator.cpp
 *
 *  Created on: 2013-3-7
 *      Author: rogeryang
 */
#include <set>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include "cpp_service_helpers.h"
#include "cpp_service_generator.h"

namespace google {
namespace protobuf {
namespace compiler {

string GetSppMainFileName( const FileDescriptor* file )
{
    string name = cpp_service::ToFileName( file->package() );
    if( name.size() > 0 )
    {
        return "spp_" + name + "_main.cc";
    }
    else
    {
        return "spp_main.cc";
    }
}

string GetSppMainObjName( const FileDescriptor* file )
{
    string name = cpp_service::ToFileName( file->package() );
    if( name.size() > 0 )
    {
        return "spp_" + name + "_main.o";
    }
    else
    {
        return "spp_main.o";
    }
}

string GetBaseDefHFileName( const FileDescriptor* file )
{
    string name = cpp_service::ToFileName( file->package() );
    if( name.size() > 0 )
    {
        return name + "_def.h";
    }
    else
    {
        return "base_def.h";
    }
}

string GetServiceApiHFileName( const FileDescriptor* file )
{
    string name = cpp_service::ToFileName( file->service(0)->full_name() );
    return name + "_api.h";
}

string GetServiceApiCCFileName ( const FileDescriptor* file )
{
    string name = cpp_service::ToFileName( file->service(0)->full_name() );
    return name + "_api.cc";
}

string GetServiceApiObjFileName ( const FileDescriptor* file )
{
    string name = cpp_service::ToFileName( file->service(0)->full_name() );
    return name + "_api.o";
}

string GetServiceApiReleaseName( const FileDescriptor* file )
{
    string name = cpp_service::ToFileName( file->service(0)->full_name() );
    return "lib" + name + "_api.a";
}

string GetServiceHFileName( const FileDescriptor* file )
{
    string name = cpp_service::ToFileName( file->service(0)->full_name() );
    return name + ".h";
}

string GetServiceCCFileName( const FileDescriptor* file )
{
    string name = cpp_service::ToFileName( file->service(0)->full_name() );
    return name + ".cc";
}

string GetServiceObjFileName( const FileDescriptor* file )
{
    string name = cpp_service::ToFileName( file->service(0)->full_name() );
    return name + ".o";
}

string GetSppSoFileName( const FileDescriptor* file )
{
    string name = cpp_service::ToFileName( file->service(0)->full_name() );
    return "spp_" + name + ".so";
}

string GetPbHFileName( const FileDescriptor* file )
{
    string name = cpp_service::StripProto(file->name());
    return name + ".pb.h";
}

string GetPbObjFileName( const FileDescriptor* file )
{
    string name = cpp_service::StripProto(file->name());
    return name + ".pb.o";
}

void PrintNameSpaceOpen( const FileDescriptor* file,
                         io::Printer& printer )
{
    vector<string> name_spaces = cpp_service::GetNameSpace(file);
    for( uint32_t i=0; i<name_spaces.size(); ++i )
    {
        printer.Print(
            "namespace $namespace$\n"
            "{\n",
            "namespace", name_spaces[i]
                );
    }
}

void PrintNameSpaceClose( const FileDescriptor* file,
                          io::Printer& printer )
{
    vector<string> name_spaces = cpp_service::GetNameSpace(file);
    for( uint32_t i=0; i<name_spaces.size(); ++i )
    {
        printer.Print(
            "}\n"
                );
    }
}

bool
CppServiceCodeGenerator::Generate(
        const FileDescriptor* file, const string& parameter,
        GeneratorContext* generator_context, string* error ) const
{
    if( !cpp_service::HasGenericServices(file) )
    {
        return true;
    }

    //only support one service now
    if ( file->service_count()!= 1 )
    {
        fprintf( stderr, "service count must be 1\n" );
        return false;
    }

    /* get the file name */
    string spp_main_cc_file = GetSppMainFileName(file);
    string base_def_h_file = GetBaseDefHFileName(file);
    string service_h_file = GetServiceHFileName(file);
    string service_cc_file = GetServiceCCFileName(file);
    string pb_h_file = GetPbHFileName(file);
    string service_api_h_file = GetServiceApiHFileName(file);
    string service_api_cc_file = GetServiceApiCCFileName(file);

    string tmp_err = "";
    if( !GenerateSppMainCC(spp_main_cc_file, generator_context,
                           file, tmp_err) )
    {
        *error = "GenerateSppMain error:" + tmp_err;
        return false;
    }

    if( !GenerateBaseDefH(base_def_h_file, generator_context,
                           file, tmp_err) )
    {
        *error = "GenerateBaseDefH error:" + tmp_err;
        return false;
    }

    if( !GenerateServiceH(service_h_file, generator_context,
                          file, tmp_err) )
    {
        *error = "GenerateServiceH error:" + tmp_err;
        return false;
    }

    if( !GenerateServiceCC(service_cc_file, generator_context,
                           file, tmp_err) )
    {
        *error = "GenerateServiceCC error:" + tmp_err;
        return false;
    }

    if( !GenerateServiceApiH(service_api_h_file, generator_context,
                             file, tmp_err) )
    {
        *error = "GenerateServiceApiH error:" + tmp_err;
        return false;
    }

    if( !GenerateServiceApiCC(service_api_cc_file, generator_context,
                              file, tmp_err) )
    {
        *error = "GenerateServiceApiCC error:" + tmp_err;
        return false;
    }

    if( !GenerateMakefile("Makefile", generator_context,
                          file, tmp_err) )
    {
        *error = "GenerateMakefile error:" + tmp_err;
        return false;
    }

    return true;
}

bool
CppServiceCodeGenerator::GenerateSppMainCC(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* file, string& err ) const
{
    scoped_ptr<io::ZeroCopyOutputStream> output(
    generator_context->Open(cc_file));
    io::Printer printer(output.get(), '$');

    //include
    printer.Print(
        "#include \"sppincl.h\"\n"
        "#include <sys/socket.h>\n"
        "#include <netinet/in.h>\n"
        "#include <arpa/inet.h>\n"
        "#include <string>\n"
        "#include \"vrpc/base/mem_tools.h\"\n"
        "#include \"vrpc/protocol/protocol.h\"\n"
        "#include \"vrpc/protocol/vrpc_head.pb.h\"\n"
    		);

    for (int i = 0; i < file->dependency_count(); i++)
    {
      printer.Print(
        "#include \"$dependency$\"\n",
        "dependency", GetPbHFileName(file)
        );
    }

    map<string, string> vars_;
    vars_["base_def_h"] = GetBaseDefHFileName(file);
    vars_["service_h"] = GetServiceHFileName(file);
    printer.Print( vars_,
        "#include \"$base_def_h$\"\n"
        "#include \"$service_h$\"\n"
        "\n"
        );

    //use namespace
    printer.Print(
        "using namespace std;\n"
        "\n"
        );

    //global vars
    const ServiceDescriptor* service = file->service(0);
    assert( service );
    printer.Print(
        "$service_name$ g_cService;\n"
        "\n",
        "service_name", cpp_service::ToClassName(service->full_name())
        );

    //forward declare
    printer.Print(
        "char *format_time( time_t tm);\n"
        "\n"
        "int send_response( const vrpc::protocol::head& cHead,\n"
        "google::protobuf::Message* pcResp,\n"
        "CTCommu* comm, unsigned flow, CServerBase* base );\n"
        "\n"
        "int unpack_req( const char* buf, size_t buf_len, vrpc::protocol::head& head,\n"
        "       google::protobuf::Message*& req );\n"
        "\n"
        );

    //handle_init
    printer.Print(
        "extern \"C\" int spp_handle_init(void* arg1, void* arg2)\n"
        "{\n"
        "    //插件自身的配置文件\n"
        "    const char* etc = (const char*)arg1;\n"
        "    //服务器容器对象\n"
        "    CServerBase* base = (CServerBase*)arg2;\n"
        "\n"
        "    base->log_.LOG_P_PID(LOG_DEBUG, \"spp_handle_init, config:%s, servertype:%d\\n\", etc, base->servertype());\n"
        "\n"
        "    int pid_type = base->servertype();\n"
        "    int iRet = 0;\n"
        "\n"
        "        //initialize configuration\n"
        "    switch (pid_type)\n"
        "    {\n"
        "    case SERVER_TYPE_PROXY:\n"
        "    case SERVER_TYPE_CTRL:\n"
        "        break;\n"
        "    case SERVER_TYPE_WORKER:\n"
        "        iRet = g_cService.InitWithConf(etc);\n"
        "        if( iRet != 0 )\n"
        "        {\n"
        "            base->log_.LOG_P_PID( LOG_ERROR, \"spp_handle_init InitWithConf error.ret:%d err:%s\\n\",\n"
        "                                  iRet, g_cService.GetErrorMessage() );\n"
        "            return -1;\n"
        "        }\n"
        "        break;\n"
        "    default:\n"
        "        return -1;\n"
        "        break;\n"
        "    }\n"
        "\n"
        "    return 0;\n"
        "}\n"
        "\n"
        );

    //handle_input
    printer.Print(
        "extern \"C\" int spp_handle_input(unsigned flow, void* arg1, void* arg2)\n"
        "{\n"
        "    //数据块对象，结构请参考tcommu.h\n"
        "    blob_type* blob = (blob_type*)arg1;\n"
        "    //extinfo有扩展信息\n"
        "    TConnExtInfo* extinfo = (TConnExtInfo*)blob->extdata;\n"
        "    //服务器容器对象\n"
        "    CServerBase* base = (CServerBase*)arg2;\n"
        "\n"
        "/* 通过profile的分析表明，这里format_time的调用在业务逻辑很轻的情况下，会成为性能瓶颈，如果你确实需要，可以取消这里的注释  */\n"
        "/*\n"
        "    base->log_.LOG_P(LOG_DEBUG, \"spp_handle_input[recv time:%s] flow:%d, buffer len:%d, client ip:%s\\n\",\n"
        "                     format_time(extinfo->recvtime_),\n"
        "                     flow,\n"
        "                     blob->len,\n"
        "                     inet_ntoa(*(struct in_addr*)&extinfo->remoteip_));\n"
        "*/\n"
        "\n"
        "    if( blob->len < vrpc::protocol::PKG_LENGTH_LEN )\n"
        "    {\n"
        "        return 0;\n"
        "    }\n"
        "\n"
        "    uint32_t len = *(uint32_t*)(blob->data);\n"
        "\n"
        "    return ntohl(len);\n"
        "}\n"
        "\n"
        );

    //handle_route
    printer.Print(
"//路由选择（可选实现）\n"
"//flow: 请求包标志\n"
"//arg1: 数据块对象\n"
"//arg2: 服务器容器对象\n"
"//返回值表示worker的组号\n"
"extern \"C\" int spp_handle_route(unsigned flow, void* arg1, void* arg2)\n"
"{\n"
"    //服务器容器对象\n"
"    CServerBase* base = (CServerBase*)arg2;\n"
"    base->log_.LOG_P_FILE(LOG_DEBUG, \"spp_handle_route, flow:%d\\n\", flow);\n"
"    return 1;\n"
"}\n"
    );

    //handle_process
    printer.Print(
"//数据处理（必须实现）\n"
"//flow: 请求包标志\n"
"//arg1: 数据块对象\n"
"//arg2: 服务器容器对象\n"
"//返回0表示成功，非0失败（将会主动断开连接）\n"
"extern \"C\" int spp_handle_process(unsigned flow, void* arg1, void* arg2)\n"
"{\n"
"    //数据块对象，结构请参考tcommu.h\n"
"    blob_type* blob = (blob_type*)arg1;\n"
"    //数据来源的通讯组件对象\n"
"    CTCommu* commu = (CTCommu*)blob->owner;\n"
"    //extinfo有扩展信息\n"
"    TConnExtInfo* extinfo = (TConnExtInfo*)blob->extdata;\n"
"    //服务器容器对象\n"
"    CServerBase* base = (CServerBase*)arg2;\n"
"\n"
"/* 通过profile的分析表明，这里format_time的调用在业务逻辑很轻的情况下，会成为性能瓶颈，如果你确实需要，可以取消这里的注释  */\n"
"/*\n"
"    base->log_.LOG_P_PID(LOG_DEBUG, \"spp_handle_process[recv time:%s] flow:%d, buffer len:%d, client ip:%s\\n\",\n"
"                         format_time(extinfo->recvtime_),\n"
"                         flow,\n"
"                         blob->len,\n"
"                         inet_ntoa(*(struct in_addr*)&extinfo->remoteip_));\n"
"*/\n"
"\n"
"   if( blob->len < 2*vrpc::protocol::PKG_LENGTH_LEN )\n"
"   {\n"
"       base->log_.LOG_P_PID(LOG_ERROR, \"spp_handle_process blob->len is too small: %u\\n\", blob->len );\n"
"       return -1;\n"
"   }\n"
"\n"
"    const char* p_body = NULL;\n"
"    uint32_t body_len = 0;\n"
"    vrpc::protocol::head cHead;\n"
"    int iRet = vrpc::protocol::unpack_head( blob->data, blob->len, cHead, p_body, body_len );\n"
"    if( (iRet!=0) || (p_body==NULL) || (body_len==0) )\n"
"    {\n"
"        base->log_.LOG_P_PID(LOG_ERROR, \"spp_handle_process unpack_head error.ret:%d\\n\", iRet );\n"
"        return -1;\n"
"    }\n"
"\n"
"    g_cService.Reset();"
"\n"
            );

    //req, resp define
    for( int32_t i=0; i<service->method_count(); ++i )
    {
        map<string, string> vars;
        vars["req_class"] = cpp_service::GetMethodInClass(service->method(i));
        vars["req_name"] = cpp_service::GetMethodReqName(service->method(i));
        vars["resp_class"] = cpp_service::GetMethodOutClass(service->method(i));
        vars["resp_name"] = cpp_service::GetMethodRespName(service->method(i));
        printer.Print(
                vars,
"    $req_class$ $req_name$;\n"
"    $resp_class$ $resp_name$;\n"
                );
    }
    printer.Print("google::protobuf::Message* pcResp = NULL;\n");

    printer.Print(
"    switch( cHead.cmd() )\n"
"    {\n"
            );
    for( int32_t i=0; i<service->method_count(); ++i )
    {
        map<string, string> vars;
        vars["method_cmd"] = cpp_service::GetMethodCmd( service->method(i) );
        vars["method_in_class"] = cpp_service::GetMethodInClass( service->method(i) );
        vars["method_out_class"] = cpp_service::GetMethodOutClass( service->method(i) );
        vars["method_name"] = service->method(i)->name();
        vars["method_req_name"] = cpp_service::GetMethodReqName(service->method(i));
        vars["method_resp_name"] = cpp_service::GetMethodRespName(service->method(i));
        printer.Print( vars,
        "    case $method_cmd$:\n"
        "        if( !$method_req_name$.ParseFromArray(p_body, body_len) )\n"
        "        {\n"
        "            base->log_.LOG_P_PID(LOG_ERROR, \"spp_handle_process pase req error.$method_req_name$ cmd:%u\\n\", cHead.cmd() );\n"
        "            return -1;\n"
        "        }\n"
        "        pcResp = &$method_resp_name$;\n"
        "        iRet = g_cService.$method_name$( $method_req_name$, $method_resp_name$ );\n"
        "        break;\n"
                );
    }
    printer.Print(
    "    default:\n"
    "        base->log_.LOG_P_PID(LOG_ERROR, \"spp_handle_process unknow cmd:%u\\n\", cHead.cmd() );\n"
    "        return -1;\n"
    "    }\n"
    "\n"
            );

    printer.Print(
    "    if( iRet != 0 )\r\n"
    "    {\r\n"
    "        //todo 应该要上报统计或者监控\r\n"
    "        base->log_.LOG_P_PID(LOG_ERROR, \"spp_handle_process service return error.ret:%d err:%s\\n\",\r\n"
    "                             iRet, g_cService.GetErrorMessage() );\r\n"
    "    }\n"
    "\n"
    "    iRet = send_response( cHead, pcResp, commu, flow, base );\n"
    "    if( iRet != 0 )\n"
    "    {\n"
    "        base->log_.LOG_P_PID(LOG_ERROR, \"send response error, ret:%d\\n\", iRet);\n"
    "        return -1;\n"
    "    }\n"
    "\n"
            );

    printer.Print(
    "    return 0;\n"
    "}\n"
    "\n"
            );

    //other function
    printer.Print(
            "//析构资源（可选实现）\n"
            "//arg1: 保留参数\n"
            "//arg2: 服务器容器对象\n"
            "extern \"C\" void spp_handle_fini(void* arg1, void* arg2)\n"
            "{\n"
            "    //服务器容器对象\n"
            "    CServerBase* base = (CServerBase*)arg2;\n"
            "\n"
            "    base->log_.LOG_P_PID(LOG_DEBUG, \"spp_handle_fini\\n\");\n"
            "}\n"
            "\n"
            "char *format_time( time_t tm)\n"
            "{\n"
            "    static char str_tm[1024];\n"
            "    struct tm tmm;\n"
            "    memset(&tmm, 0, sizeof(tmm) );\n"
            "    localtime_r((time_t *)&tm, &tmm);\n"
            "\n"
            "    snprintf(str_tm, sizeof(str_tm), \"[%04d-%02d-%02d %02d:%02d:%02d]\",\n"
            "             tmm.tm_year + 1900, tmm.tm_mon + 1, tmm.tm_mday,\n"
            "             tmm.tm_hour, tmm.tm_min, tmm.tm_sec);\n"
            "\n"
            "    return str_tm;\n"
            "}\n"
            "\n"
            "int send_response( const vrpc::protocol::head& cHead,\n"
            "                   google::protobuf::Message* pcResp,\n"
            "                   CTCommu* comm, unsigned flow, CServerBase* base )\n"
            "{\n"
            "    blob_type response;\n"
            "\n"
            "    pack_message( cHead, *pcResp, response.data, response.len );\n"
            "\n"
            "    //send response\n"
            "    int ret = comm->sendto( flow, &response, (void*)base );\n"
            "    free( response.data );\n"
            "\n"
            "    return ret;\n"
            "}\n"
            "\n"
            );

    return true;
}

bool
CppServiceCodeGenerator::GenerateBaseDefH(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* file, string& err ) const
{
    scoped_ptr<io::ZeroCopyOutputStream> output(
    generator_context->Open(cc_file));
    io::Printer printer(output.get(), '$');

    const ServiceDescriptor* service = file->service(0);

    printer.Print(
            "#ifndef $h_macro$\n"
            "#define $h_macro$\n"
            "\n",
            "h_macro", cpp_service::GetHMacroName(cc_file)
            );

    //namespace open
    PrintNameSpaceOpen( file, printer );
    printer.Print("\n");

    //cmd define
    printer.Print(
            "/* cmd define */\n"
            "enum\n"
            "{\n"
            );
    for( int i=0; i<service->method_count(); ++i )
    {
        if( i == service->method_count()-1 )
        {
            printer.Print(
                    "    $cmd$\n",
                    "cmd", cpp_service::GetMethodCmdNoNS(service->method(i))
                    );
        }
        else
        {
            printer.Print(
                    "    $cmd$,\n",
                    "cmd", cpp_service::GetMethodCmdNoNS(service->method(i))
                    );
        }
    }
    printer.Print("};\n");

    printer.Print("\n");

    //namespace close
    PrintNameSpaceClose( file, printer );

    printer.Print(
            "#endif /* $h_macro$ */\n",
            "h_macro", cpp_service::GetHMacroName(cc_file)
    );

    return true;
}

bool
CppServiceCodeGenerator::GenerateServiceH(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* file, string& err ) const
{
    scoped_ptr<io::ZeroCopyOutputStream> output(
    generator_context->Open(cc_file));
    io::Printer printer(output.get(), '$');

    const ServiceDescriptor* service = file->service(0);

    printer.Print(
            "#ifndef $h_macro$\n"
            "#define $h_macro$\n"
            "\n",
            "h_macro", cpp_service::GetHMacroName(cc_file)
            );

    //include
    printer.Print(
            "#include <stdint.h>\n"
            "#include \"vrpc/base/CService.h\"\n"
            "#include \"$pb_file$\"\n"
            "\n",
            "pb_file", GetPbHFileName(file)
            );

    //forward declare
    set<string> type_set;
    for( int i=0; i<service->method_count(); ++i )
    {
        string in_type = cpp_service::GetMethodInClass(service->method(i));
        string out_type = cpp_service::GetMethodOutClass(service->method(i));
        type_set.insert(in_type);
        type_set.insert(out_type);
    }
    for( set<string>::iterator iter=type_set.begin();
         iter!=type_set.end(); ++iter )
    {
        printer.Print(
                "class $classname$;\n",
                "classname", *iter
                );
    }
    printer.Print("\n");

    //namespace open
    PrintNameSpaceOpen( file, printer );
    printer.Print("\n");

    //class def
    printer.Print(
            "class $service_name$ : public vrpc::base::CService\n"
            "{\n"
            "public:\n"
            "    $service_name$();\n"
            "    virtual\n"
            "    ~$service_name$();\n"
            "\n"
            "    int InitWithConf( const char* pchConfFile );\n"
            "\n"
            "    void Reset();\n"
            "\n",
            "service_name", service->name()
            );
    //method
    for( int i=0; i<service->method_count(); ++i )
    {
        printer.Print(
                "    $method_declare$;\n"
                "\n",
                "method_declare", cpp_service::GetMethodDeclare(service->method(i))
                );
    }
    printer.Print(
            "\n"
            "};\n"
            );

    //namespace close
    printer.Print("\n");
    PrintNameSpaceClose( file, printer );

    printer.Print(
            "#endif /* $h_macro$ */\n",
            "h_macro", cpp_service::GetHMacroName(cc_file)
    );

    return true;
}

bool
CppServiceCodeGenerator::GenerateServiceCC(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* file, string& err ) const
{
    scoped_ptr<io::ZeroCopyOutputStream> output(
    generator_context->Open(cc_file));
    io::Printer printer(output.get(), '$');

    const ServiceDescriptor* service = file->service(0);

    //include
    printer.Print(
            "#include \"$head$\"\n",
            "head", GetServiceHFileName(file)
            );
    printer.Print("\n");

    //namespace open
    PrintNameSpaceOpen( file, printer );
    printer.Print("\n");

    //fixed methods
    printer.Print(
            "    $service_name$::$service_name$()\n"
            "    {\n"
            "\n"
            "    }\n"
            "\n"
            "    $service_name$::~$service_name$()\n"
            "    {\n"
            "\n"
            "    }\n"
            "\n"
            "    void $service_name$::Reset()\n"
            "    {\n"
            "        CService::Reset();\n"
            "    }\n"
            "\n"
            "    int32_t $service_name$::InitWithConf( const char* pchConfFile )\n"
            "    {\n"
            "        return 0;\n"
            "    }\n"
            "\n",
            "service_name", service->name()
            );

    printer.Indent();
    printer.Indent();
    //user define methods
    for( int i=0; i<service->method_count(); ++i )
    {
        printer.Print(
                "$method_imp$"
                "\n",
                "method_imp", cpp_service::GetMethodImp(service->method(i))
                );
    }

    //namespace close
    printer.Outdent();
    printer.Outdent();
    printer.Print("\n");
    PrintNameSpaceClose( file, printer );

    return true;
}


bool
CppServiceCodeGenerator::GenerateMakefile(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* file, string& err ) const
{
    scoped_ptr<io::ZeroCopyOutputStream> output(
    generator_context->Open(cc_file));
    io::Printer printer(output.get(), '!');

    map<string, string> vars;
    vars["spp_so_name"] = GetSppSoFileName(file);
    vars["service_obj_name"] = GetServiceObjFileName(file);
    vars["pb_obj_name"] = GetPbObjFileName(file);
    vars["spp_obj_name"] = GetSppMainObjName(file);
    vars["api_obj_name"] = GetServiceApiObjFileName(file);
    vars["api_release_name"] = GetServiceApiReleaseName(file);

    printer.Print( vars,
            "ARCH:=$(shell uname -m)\n"
            "BIT32:=i686\n"
            "BIT64:=x86_64\n"
            "\n"
            "CC   = gcc\n"
            "CXX  = g++\n"
            "AR   = ar\n"
            "\n"
            "C_FLAGS= -Wall -O2 -g -fPIC\n"
            "SPP_LINK_FLAGS = -shared\n"
            "\n"
            "INCL_SPP_64 = -I../include/spp/spp_2.7.1_64\n"
            "INCL_SPP_32 = -I../include/spp/spp_2.7.1_32\n"
            "INCL_CLIB = -I../include/clib\n"
            "INCL_L5 = -I../include/l5\n"
            "\n"
            "LIB_64 = -L../lib/64bit\n"
            "LIB_32 = -L../lib/32bit\n"
            "\n"
            "ifeq ($(ARCH),$(BIT64))\n"
            "    INCL_SPP = $(INCL_SPP_64)\n"
            "    LIB_PATH = $(LIB_64)\n"
            "else\n"
            "    INCL_SPP = $(INCL_SPP_32)\n"
            "    LIB_PATH = $(LIB_32)\n"
            "endif\n"
            "\n"
            "INCL_PROJ = -I./\n"
            "INCL = $(INCL_SPP) $(INCL_PROJ) $(INCL_CLIB) $(INCL_L5)\n"
            "SPP_LIB = -lprotobuf\n"
            "SPP_INSTALL_DIR = ../release/spp_so/\n"
            "API_INSTALL_DIR = ../release/api/\n"
            "\n"
            "VRPC_OBJS= vrpc/base/CObject.o vrpc/protocol/vrpc_head.pb.o\n"
            "VRPC_API_OBJS= vrpc/base/CApiBase.o vrpc/base/CObject.o vrpc/protocol/vrpc_head.pb.o\n"
            "\n"
            "SPP_TARGET= !spp_so_name!\n"
            "SPP_TARGET_OBJECTS= !spp_obj_name! !service_obj_name! !pb_obj_name! $(VRPC_OBJS)\n"
            "\n"
            "API_TARGET= !api_release_name!\n"
            "API_TARGET_OBJECTS= !api_obj_name! !pb_obj_name! $(VRPC_API_OBJS)\n"
            "\n"
            "ALL_TARGET= $(SPP_TARGET) $(API_TARGET)\n"
            "ALL_OBJECTS= $(SPP_TARGET_OBJECTS) $(API_TARGET_OBJECTS)\n"
            "\n"
            "all : $(ALL_TARGET)\n"
            "\n"
            "$(SPP_TARGET) : $(SPP_TARGET_OBJECTS)\n"
            "	$(CXX) $(SPP_LINK_FLAGS) -o $(SPP_TARGET) $(SPP_TARGET_OBJECTS) $(SPP_LIB)\n"
            "\n"
            "$(API_TARGET) : $(API_TARGET_OBJECTS)\n"
            "	$(AR) -r $(API_TARGET) $(API_TARGET_OBJECTS)\n"
            "\n"
            "%.o : %.cc\n"
            "	$(CXX) -c $(C_FLAGS) -o $@ $< $(INCL)\n"
            "%.o : %.cpp\n"
            "	$(CXX) -c $(C_FLAGS) -o $@ $< $(INCL)\n"
            "\n"
            "clean:\n"
            "	rm -f $(ALL_TARGET) $(ALL_OBJECTS)\n"
            "install:\n"
            "	cp $(SPP_TARGET) $(SPP_INSTALL_DIR)\n"
            "	cp $(API_TARGET) $(API_INSTALL_DIR)/lib/\n"
            );

    return true;
}

bool
CppServiceCodeGenerator::GenerateServiceApiH(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* file, string& err ) const
{
    scoped_ptr<io::ZeroCopyOutputStream> output(
    generator_context->Open(cc_file));
    io::Printer printer(output.get(), '$');

    const ServiceDescriptor* service = file->service(0);

    printer.Print(
            "#ifndef $h_macro$\n"
            "#define $h_macro$\n"
            "\n",
            "h_macro", cpp_service::GetHMacroName(cc_file)
            );

    //include
    printer.Print(
            "#include \"vrpc/base/CApiBase.h\"\n"
            "#include \"$pb_file$\"\n"
            "\n",
            "pb_file", GetPbHFileName(file)
            );

    //namespace open
    PrintNameSpaceOpen( file, printer );
    printer.Print("\n");

    //class def
    printer.Print(
            "class $service_api_name$ : public vrpc::base::CApiBase\n"
            "{\n"
            "public:\n"
            "    $service_api_name$();\n"
            "    virtual\n"
            "    ~$service_api_name$();\n"
            "\n",
            "service_api_name", cpp_service::GetServiceApiClassName(service)
            );
    //method
    for( int i=0; i<service->method_count(); ++i )
    {
        printer.Print(
                "    $method_declare$;\n"
                "\n",
                "method_declare", cpp_service::GetMethodDeclare(service->method(i))
                );
    }
    printer.Print(
            "\n"
            "};\n"
            );

    //namespace close
    printer.Print("\n");
    PrintNameSpaceClose( file, printer );

    printer.Print(
            "#endif /* $h_macro$ */\n",
            "h_macro", cpp_service::GetHMacroName(cc_file)
    );

    return true;
}

bool
CppServiceCodeGenerator::GenerateServiceApiCC(
        const string& cc_file, GeneratorContext* generator_context,
        const FileDescriptor* file, string& err ) const
{
    scoped_ptr<io::ZeroCopyOutputStream> output(
    generator_context->Open(cc_file));
    io::Printer printer(output.get(), '$');

    const ServiceDescriptor* service = file->service(0);

    //include
    map<string, string> vars;
    vars["api_h"] = GetServiceApiHFileName(file);
    vars["base_def_h"] = GetBaseDefHFileName(file);
    printer.Print(
            vars,
            "#include <stdio.h>\n"
            "#include <errno.h>\n"
            "#include \"$api_h$\"\n"
            "#include \"$base_def_h$\"\n"
            );
    printer.Print("\n");

    //namespace open
    PrintNameSpaceOpen( file, printer );
    printer.Print("\n");

    //fixed methods
    printer.Print(
            "    $service_api_name$::$service_api_name$()\n"
            "    {\n"
            "\n"
            "    }\n"
            "\n"
            "    $service_api_name$::~$service_api_name$()\n"
            "    {\n"
            "\n"
            "    }\n"
            "\n",
            "service_api_name", cpp_service::GetServiceApiClassName(service)
            );

    printer.Indent();
    printer.Indent();
    //user define methods
    for( int i=0; i<service->method_count(); ++i )
    {
        map<string, string> vars;
        vars["service_api_name"] = cpp_service::GetServiceApiClassName(service);
        vars["method_name"] = service->method(i)->name();
        vars["method_in_class"] = cpp_service::GetMethodInClass(service->method(i));
        vars["method_out_class"] = cpp_service::GetMethodOutClass(service->method(i));
        vars["method_cmd"] = cpp_service::GetMethodCmd(service->method(i));
        printer.Print(
                vars,
                "int32_t $service_api_name$::$method_name$( const $method_in_class$& cReq, $method_out_class$& cResp )\n"
                "{\n"
                "        int iRet = CallServer( $method_cmd$,\n"
                "                               cReq, cResp );\n"
                "        if( iRet != 0 )\n"
                "        {\n"
                "            return iRet;\n"
                "        }\n"
                "\n"
                "        return 0;\n"
                "}\n"
                "\n"
                );
    }

    //namespace close
    printer.Outdent();
    printer.Outdent();
    printer.Print("\n");
    PrintNameSpaceClose( file, printer );

    return true;
}

}//namespace compiler
} //namespace protobuf
} //namespace google
