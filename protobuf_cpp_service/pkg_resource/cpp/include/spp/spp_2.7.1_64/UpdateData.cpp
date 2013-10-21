/*
 * =====================================================================================
 *
 *       Filename:  UpdateData.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/21/2010 03:04:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericsha (shakaibo), ericsha@tencent.com
 *        Company:  Tencent, China
 *
 * =====================================================================================
 */

#include "UpdateData.h"
#include "msg.h"
#include "sppincl.h"

extern int seq;

// 请求打包
int CUpdateData::HandleEncode( CAsyncFrame *pFrame,
        char *buf,
        int &len,
        CMsgBase *pMsg)
{
    CMsg *msg = (CMsg*)pMsg;
    *(int *)buf = msg->uin;
    *((int*)buf+1) = seq;
    *((int*)buf+2) = msg->level;
    *((int*)buf+3) = msg->coin;
    *((int*)buf+4) = msg->seed_num;
    *((int*)buf+5) = msg->gain_num;
    len = 6*sizeof(int);
    
    SetCurrentSeq( seq++ ); // Set when using ActionType_SendRecv_Parallel
   
    pFrame->FRAME_LOG( LOG_DEBUG, "CUpdateData::HandleEncode. seq: %d\n", *((int*)buf+1));
    return 0;
}

// 回应包完整性检查
int CUpdateData::HandleInput( CAsyncFrame *pFrame,
        const char *buf,
        int len,
        CMsgBase *pMsg)
{
    pFrame->FRAME_LOG( LOG_DEBUG, "CUpdateData::HandleInput. buf len: %d\n", len);
    if( len == sizeof(int)*3 )
    {
        int seqno = *(int*)(buf+sizeof(int));
        SetCurrentSeq( seqno ); // Set when using ActionType_SendRecv_Parallel
        return len;
    }
    return 0;
}

// 回应包处理
int CUpdateData::HandleProcess( CAsyncFrame *pFrame,
        const char *buf,
        int len,
        CMsgBase *pMsg)
{
    
    int uin = *(int*)(buf);
    int seq = *(int*)(buf+sizeof(int));
    int result = *(int*)(buf+sizeof(int)*2);

    pFrame->FRAME_LOG( LOG_DEBUG, "CUpdateData::HandleProcess. buf len: %d; seq: %d; uin: %d; result: %d\n", 
            len, seq, uin, result);

    return 0;
}


int CUpdateData::HandleError( CAsyncFrame *pFrame,
        int err_no,
        CMsgBase *pMsg)
{
    pFrame->FRAME_LOG( LOG_ERROR, "CUpdateData::HandleError. errno: %d\n", err_no);

    return 0;
}
