#include <stdio.h>
#include <errno.h>
#include "vrpc_example_example_service_api.h"
#include "vrpc/protocol/protocol.h"
#include "vrpc_example_def.h"

namespace vrpc
{
namespace example
{

    example_serviceApi::example_serviceApi()
    {

    }

    example_serviceApi::~example_serviceApi()
    {

    }

    int32_t example_serviceApi::call_1( const vrpc::example::req& cReq, vrpc::example::resp_1& cResp )
    {
            int iRet = CallServer( vrpc::example::CALL_1_CMD,
                                   cReq, cResp );
            if( iRet != 0 )
            {
                return iRet;
            }

            return 0;
    }

    int32_t example_serviceApi::call_2( const vrpc::example::req& cReq, vrpc::example::resp_2& cResp )
    {
            int iRet = CallServer( vrpc::example::CALL_2_CMD,
                                   cReq, cResp );
            if( iRet != 0 )
            {
                return iRet;
            }

            return 0;
    }


}
}
