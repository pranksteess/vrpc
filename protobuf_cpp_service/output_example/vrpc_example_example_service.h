#ifndef VRPC_EXAMPLE_EXAMPLE_SERVICE_H
#define VRPC_EXAMPLE_EXAMPLE_SERVICE_H

#include <stdint.h>
#include "vrpc/base/CService.h"
#include "output_service_example.pb.h"

class vrpc::example::req;
class vrpc::example::resp_1;
class vrpc::example::resp_2;

namespace vrpc
{
namespace example
{

class example_service : public vrpc::base::CService
{
public:
    example_service();
    virtual
    ~example_service();

    int InitWithConf( const char* pchConfFile );

    void Reset();

    int32_t call_1( const vrpc::example::req& req,
    vrpc::example::resp_1& resp );

    int32_t call_2( const vrpc::example::req& req,
    vrpc::example::resp_2& resp );


};

}
}
#endif /* VRPC_EXAMPLE_EXAMPLE_SERVICE_H */
