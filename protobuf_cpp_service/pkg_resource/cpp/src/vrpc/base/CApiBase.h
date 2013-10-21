/*
 * CApiBase.h
 *
 *  Created on: 2013-5-17
 *      Author: ylc8131
 */

#ifndef CAPIBASE_H_
#define CAPIBASE_H_

#include <string>
#include <sys/socket.h>
#include "vrpc/base/CObject.h"

using namespace std;

//forward declare
class clib_sock_l5;
namespace google
{
namespace protobuf
{
    class Message;
}
}
namespace vrpc
{
namespace protocol
{
    class head;
}
}

namespace vrpc
{
namespace base
{

class CApiBase : public CObject
{
public:
    CApiBase();
    virtual
    ~CApiBase();

    int32_t Init( int iL5ModId, int iL5CmdId, int iLongConn, int iTimeOut, int iConnProto = SOCK_STREAM );

    int32_t SetDefaultServer( const char *pchIp, const int iPort );

protected:
    static uint32_t m_uiSeq;
    clib_sock_l5* m_pcSock;

    int32_t CallServer( int32_t iCmd, const google::protobuf::Message& cReqBody,
                        google::protobuf::Message& cRespBody );
    int32_t SendAndRecvPb( const vrpc::protocol::head& cReqHead,
                           const google::protobuf::Message& cReqBody,
                           vrpc::protocol::head& cRespHead,
                           google::protobuf::Message& cRespBody );
    int32_t SendAndRecvPbStr( const char* pcBuf, int iBufLen, string& strResp );
};

} /* namespace base */
} /* namespace vrpc */
#endif /* CAPIBASE_H_ */
