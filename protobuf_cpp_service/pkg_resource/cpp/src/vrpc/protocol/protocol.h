#ifndef _PROTOCOL_DEFINE_H_
#define _PROTOCOL_DEFINE_H_
#include <string>
#include <arpa/inet.h>
#include "vrpc/protocol/vrpc_head.pb.h"

using std::string;

namespace vrpc
{
namespace protocol
{

const int32_t PKG_LENGTH_LEN = sizeof(uint32_t);
const uint32_t VRPC_VER_1 = 1;
const uint32_t BUF_LEN = 10240;

int unpack_head( const char* buf, uint32_t buf_len, vrpc::protocol::head& head,
                 const char*& body, uint32_t& body_len )
{
    //reqbuf structure: pkglen(4byte) + headlen(4byte) + head(protobuf) + body(protobuf)
    uint32_t pkg_len = ntohl(*(uint32_t*)buf);
    uint32_t head_len = ntohl(*(uint32_t*)(buf+PKG_LENGTH_LEN));
    body_len = pkg_len - 2*PKG_LENGTH_LEN - head_len;

    const void* p_head = buf+2*PKG_LENGTH_LEN;
    body = buf+2*PKG_LENGTH_LEN+head_len;

    if( head_len > pkg_len )
    {
        return -10;
    }

    //decode head
    if( !head.ParseFromArray(p_head, head_len) )
    {
        return -20;
    }

    return 0;
}

int unpack_message( const string& strBuf, vrpc::protocol::head& head,
                    google::protobuf::Message& body )
{
    const char* body_ptr = NULL;
    uint32_t body_len = 0;
    int ret = unpack_head( strBuf.data(), strBuf.size(), head, body_ptr, body_len );
    if( ret != 0 )
    {
        return ret;
    }

    if( !body.ParseFromArray(body_ptr, body_len) )
    {
        return -50;
    }

    return 0;
}

void pack_message( const vrpc::protocol::head& cHead,
                   const google::protobuf::Message& cBody,
                   char*& pcBuf, int& iLen )
{
    //reqbuf structure: pkglen(4byte) + headlen(4byte) + head(protobuf) + body(protobuf)
    int iHeadLen = cHead.ByteSize();
    int iBodyLen = cBody.ByteSize();
    int iPkgLen = 2*PKG_LENGTH_LEN+iHeadLen+iBodyLen;
    iLen = iPkgLen;

    uint32_t uiNPkgLen = htonl(iPkgLen);
    uint32_t uiNHeadLen = htonl(iHeadLen);

    pcBuf = (char*)malloc(iPkgLen);

    memcpy( pcBuf, (char*)&uiNPkgLen, PKG_LENGTH_LEN );
    memcpy( pcBuf+PKG_LENGTH_LEN, (char*)&uiNHeadLen, PKG_LENGTH_LEN );
    cHead.SerializeToArray( pcBuf+2*PKG_LENGTH_LEN, iHeadLen );
    cBody.SerializeToArray( pcBuf+2*PKG_LENGTH_LEN+iHeadLen, iBodyLen );
}

}
}

#endif

