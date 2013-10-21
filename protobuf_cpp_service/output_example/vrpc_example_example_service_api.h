#ifndef VRPC_EXAMPLE_EXAMPLE_SERVICE_API_H
#define VRPC_EXAMPLE_EXAMPLE_SERVICE_API_H

#include "vrpc/base/CApiBase.h"
#include "clib_sock_l5.h"
#include "output_service_example.pb.h"

namespace vrpc
{
namespace example
{

class example_serviceApi : public vrpc::base::CApiBase
{
public:
    example_serviceApi();
    virtual
    ~example_serviceApi();

    int32_t call_1( const vrpc::example::req& req,
    vrpc::example::resp_1& resp );

    int32_t call_2( const vrpc::example::req& req,
    vrpc::example::resp_2& resp );


};

}
}
#endif /* VRPC_EXAMPLE_EXAMPLE_SERVICE_API_H */
