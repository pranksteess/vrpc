/*
 * CApiBase.cpp
 *
 *  Created on: 2013-5-17
 *      Author: ylc8131
 */

#include <stdio.h>
#include <errno.h>
#include <string>
#include "CApiBase.h"
#include "mem_tools.h"
#include "error_def.h"
#include "vrpc/protocol/protocol.h"
#include "vrpc/protocol/vrpc_head.pb.h"
#include "clib_sock_l5.h"

using std::string;

namespace vrpc
{
namespace base
{

    uint32_t CApiBase::m_uiSeq = 0;

    CApiBase::CApiBase()
    {
        m_pcSock = new clib_sock_l5;
    }

    CApiBase::~CApiBase()
    {
        delete m_pcSock;
    }

    int32_t CApiBase::Init( int iL5ModId, int iL5CmdId, int iLongConn, int iTimeOut, int iConnProto )
    {
        int32_t iRet = 0;

        iRet = m_pcSock->init( iL5ModId, iL5CmdId, iLongConn, iTimeOut, iConnProto );
        if( iRet != 0 )
        {
            SetErrorMessage( "m_pcSock->init error.ret:%d", iRet );
            return API_SOCKINIT_ERR;
        }

        return 0;
    }

    int32_t CApiBase::SetDefaultServer( const char* pchIp, int iPort )
    {
        return m_pcSock->set_default_server( pchIp, iPort );
    }

    int32_t CApiBase::CallServer( int32_t iCmd, const google::protobuf::Message& cReqBody,
                                  google::protobuf::Message& cRespBody )
    {
        vrpc::protocol::head cReqHead;

        cReqHead.set_ver( vrpc::protocol::VRPC_VER_1 );
        cReqHead.set_cmd( iCmd );
        cReqHead.set_seq( m_uiSeq++ );

        vrpc::protocol::head cRespHead;
        int iRet = SendAndRecvPb( cReqHead, cReqBody, cRespHead, cRespBody );
        if( iRet != 0 )
        {
            return iRet;
        }

        return 0;
    }

    int32_t CApiBase::SendAndRecvPb( const vrpc::protocol::head& cReqHead,
                                     const google::protobuf::Message& cReqBody,
                                     vrpc::protocol::head& cRespHead,
                                     google::protobuf::Message& cRespBody )
    {
        string strReq;
        string strResp;
        string strl5tmp;

        char* pcBuf = NULL;
        int iBufLen = 0;
        vrpc::protocol::pack_message( cReqHead, cReqBody, pcBuf, iBufLen );
        CScopePtrGuardC cBufGuard(pcBuf);
        int iRet = SendAndRecvPbStr( pcBuf, iBufLen, strResp );
        if( iRet != 0 )
        {
            strl5tmp = "SendAndRecvPbStr is failure";
            m_pcSock->update_result(L5_ERROR, strl5tmp);
            return iRet;
        }

        strl5tmp = "SendAndRecvPbStr is ok";
        m_pcSock->update_result(L5_OK, strl5tmp);

        iRet = vrpc::protocol::unpack_message( strResp, cRespHead, cRespBody );
        if( iRet != 0 )
        {
            m_pcSock->close();
            SetErrorMessage( "unpack_message error.ret[%d]", iRet );
            return vrpc::base::API_PARSERESP_ERR;
        }

        if( cRespHead.ver()!=cReqHead.ver() ||
            cRespHead.cmd()!=cReqHead.cmd() ||
            cRespHead.seq()!=cReqHead.seq() )
        {
            SetErrorMessage( "resp head err. req[ver:%u cmd:%u seq:%u] resp[ver:%u cmd:%u seq:%u]",
                             cReqHead.ver(), cReqHead.cmd(), cReqHead.seq(),
                             cRespHead.ver(), cRespHead.cmd(), cRespHead.seq() );
            m_pcSock->close();
            return vrpc::base::API_RESPHEAD_ERR;
        }

        return 0;
    }

    int32_t CApiBase::SendAndRecvPbStr( const char* pcBuf, int iBufLen, string& strResp )
    {
        int iRet = m_pcSock->send( pcBuf, iBufLen );
        if( iRet != iBufLen )
        {
            SetErrorMessage( "m_pcSock->send error.ret[%d] should[%d] err[%s]",
                             iRet, iBufLen, m_pcSock->get_err_msg() );
            m_pcSock->close();
            return vrpc::base::API_SOCKSEND_ERR;
        }

        //recv pkglen
        char szBuf[65535];
        iRet = m_pcSock->recv( szBuf, vrpc::protocol::PKG_LENGTH_LEN );
        if( iRet != vrpc::protocol::PKG_LENGTH_LEN )
        {
            SetErrorMessage( "m_pcSock->recv pkglen error.ret[%d] should[%d] err[%s]",
                             iRet, vrpc::protocol::PKG_LENGTH_LEN, m_pcSock->get_err_msg() );
            m_pcSock->close();
            return vrpc::base::API_SOCKRECVLEN_ERR;
        }

        uint32_t uiPkgLen = ntohl(*(uint32_t*)szBuf);
        uint32_t uiLeftLen = uiPkgLen-sizeof(uint32_t);

        //check the buf size
        if( uiPkgLen > sizeof(szBuf) )
        {
            m_pcSock->close();
            SetErrorMessage( "recv buf is too small. buflen[%u] pkglen[%u]",
                             sizeof(szBuf), uiPkgLen );
            return vrpc::base::API_BUFOVERFLOW_ERR;
        }

        //recv body
        iRet = m_pcSock->recv( szBuf+vrpc::protocol::PKG_LENGTH_LEN, uiLeftLen );
        if( iRet != (int)uiLeftLen )
        {
            SetErrorMessage( "m_pcSock->recv body error.ret[%d] should[%d] err[%s]",
                             iRet, uiLeftLen, m_pcSock->get_err_msg() );
            m_pcSock->close();
            return vrpc::base::API_SOCKRECVBODY_ERR;
        }

        strResp.assign( szBuf, uiPkgLen );

        return 0;
    }


} /* namespace base */
} /* namespace vrpc */
