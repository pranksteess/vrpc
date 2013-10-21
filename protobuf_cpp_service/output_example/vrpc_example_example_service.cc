#include "vrpc_example_example_service.h"

namespace vrpc
{
namespace example
{

    example_service::example_service()
    {

    }

    example_service::~example_service()
    {

    }

    void example_service::Reset()
    {
        CService::Reset();
    }

    int32_t example_service::InitWithConf( const char* pchConfFile )
    {
        return 0;
    }

    int32_t example_service::call_1( const vrpc::example::req& req,
    vrpc::example::resp_1& resp )
    {
        return 0;
    }

    int32_t example_service::call_2( const vrpc::example::req& req,
    vrpc::example::resp_2& resp )
    {
        return 0;
    }


}
}
