#include "sppincl.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "vrpc/base/mem_tools.h"
#include "vrpc/protocol/protocol.h"
#include "vrpc/protocol/vrpc_head.pb.h"
#include "vrpc_example_def.h"
#include "vrpc_example_InfoService.h"
#include "clib_timer.h"
#include "vrpc/base/CRunRecoder.h"
#include "vrpc/base/CTnm2Reporter.h"

using namespace std;

static const int MONITOR_INTERVAL = 3;

vrpc::example::InfoService g_cService;
vrpc::base::CRunRecoder g_cRecoder;

char *format_time( time_t tm);

int send_response( const vrpc::protocol::head& cHead,
google::protobuf::Message* pcResp,
CTCommu* comm, unsigned flow, CServerBase* base );

void MonitorWorker()
{
    vrpc::base::CTnm2Reporter cReporter;
    Tnm2Attr sAttr = g_cService.GetTnm2Attr();
    cReporter.SetTnm2Attr( sAttr );
    int iRet = cReporter.Init();
    if( iRet != 0 )
    {
        fprintf( stderr, "cReporter.Init failed.ret:%d err:%s\n",
                 iRet, cReporter.GetErrorMessage() );
        exit(1);
    }

    fprintf( stderr, "MonitorWorker init ok. key[%u]\n", cReporter.GetKey() );
    fprintf( stderr, "Tnm2Attr: Req[%d] Err[%d] UsedTime[%d] AvgUsedTime[%d] ErrRatio[%d]\n",
             sAttr.iReqCountAttr, sAttr.iErrCountAttr, sAttr.iUsedTimeAttr, sAttr.iAvgUsedTimeAttr,
             sAttr.iErrRatioAttr );
    fprintf( stderr, "MonitorWoker Interval:%d\n", MONITOR_INTERVAL );

    cReporter.Run( MONITOR_INTERVAL );

    exit(0);
}

void CreateMonitorWorker()
{
    pid_t pid = fork();
    if( 0 == pid )
    {
        //child
        MonitorWorker();
    }
    else
    {
        //parent
        return;
    }
}

extern "C" int spp_handle_init(void* arg1, void* arg2)
{
    //插件自身的配置文件
    const char* etc = (const char*)arg1;
    //服务器容器对象
    CServerBase* base = (CServerBase*)arg2;

    base->log_.LOG_P_PID(LOG_DEBUG, "spp_handle_init, config:%s, servertype:%d\n", etc, base->servertype());

    char pwd[1024] = {0};
    int pid_type = base->servertype();
    int iRet = 0;
    //init service
    iRet = g_cService.InitWithConf(etc);
    if( iRet != 0 )
    {
        base->log_.LOG_P_PID( LOG_ERROR, "spp_handle_init InitWithConf error.ret:%d err:%s\n",
                              iRet, g_cService.GetErrorMessage() );
        return -1;
    }
    //init recoder
    iRet = g_cRecoder.Init();
    if( iRet != 0 )
    {
        base->log_.LOG_P_PID( LOG_ERROR, "spp_handle_init g_cRecoder.Init error.ret:%d err:%s\n",
                              iRet, g_cRecoder.GetErrorMessage() );
        return -1;
    }
    Tnm2Attr sAttr = g_cService.GetTnm2Attr();
    fprintf( stderr, "Recoder init ok.key[%u]\n", g_cRecoder.GetKey() );
    fprintf( stderr, "Tnm2Attr: Req[%d] Err[%d] UsedTime[%d] AvgUsedTime[%d] ErrRatio[%d]\n",
             sAttr.iReqCountAttr, sAttr.iErrCountAttr, sAttr.iUsedTimeAttr, sAttr.iAvgUsedTimeAttr,
             sAttr.iErrRatioAttr );

        //initialize configuration
    switch (pid_type)
    {
    case SERVER_TYPE_PROXY:
        //create the monitor process
        CreateMonitorWorker();
        break;
    case SERVER_TYPE_CTRL:
        break;
    case SERVER_TYPE_WORKER:
        break;
    default:
        return -1;
        break;
    }


    return 0;
}

extern "C" int spp_handle_input(unsigned flow, void* arg1, void* arg2)
{
    //数据块对象，结构请参考tcommu.h
    blob_type* blob = (blob_type*)arg1;
    //extinfo有扩展信息
    TConnExtInfo* extinfo = (TConnExtInfo*)blob->extdata;
    //服务器容器对象
    CServerBase* base = (CServerBase*)arg2;
/*
    base->log_.LOG_P(LOG_DEBUG, "spp_handle_input[recv time:%s] flow:%d, buffer len:%d, client ip:%s\n",
                     format_time(extinfo->recvtime_),
                     flow,
                     blob->len,
                     inet_ntoa(*(struct in_addr*)&extinfo->remoteip_));
*/
    if( blob->len < vrpc::protocol::PKG_LENGTH_LEN )
    {
        return 0;
    }

    uint32_t len = *(uint32_t*)(blob->data);

    return ntohl(len);
}

//路由选择（可选实现）
//flow: 请求包标志
//arg1: 数据块对象
//arg2: 服务器容器对象
//返回值表示worker的组号
extern "C" int spp_handle_route(unsigned flow, void* arg1, void* arg2)
{
    //服务器容器对象
    CServerBase* base = (CServerBase*)arg2;
    base->log_.LOG_P_FILE(LOG_DEBUG, "spp_handle_route, flow:%d\n", flow);
    return 1;
}

#define RETURN_VALUE(ret, report_result)\
    do{\
    uint32_t uiUsedTime = timer.end_get_ms(szProcessTimerPoint); \
    g_cRecoder.Recode( report_result, uiUsedTime );\
    return ret;\
    }while(0);

//数据处理（必须实现）
//flow: 请求包标志
//arg1: 数据块对象
//arg2: 服务器容器对象
//返回0表示成功，非0失败（将会主动断开连接）
extern "C" int spp_handle_process(unsigned flow, void* arg1, void* arg2)
{
    //数据块对象，结构请参考tcommu.h
    blob_type* blob = (blob_type*)arg1;
    //数据来源的通讯组件对象
    CTCommu* commu = (CTCommu*)blob->owner;
    //extinfo有扩展信息
    TConnExtInfo* extinfo = (TConnExtInfo*)blob->extdata;
    //服务器容器对象
    CServerBase* base = (CServerBase*)arg2;

    int iRet = 0;
    int iServiceRet = 0;
    clib_timer timer;
    char szProcessTimerPoint[] = "process";

    timer.start( szProcessTimerPoint );
/*
    base->log_.LOG_P_PID(LOG_DEBUG, "spp_handle_process[recv time:%s] flow:%d, buffer len:%d, client ip:%s\n",
                         format_time(extinfo->recvtime_),
                         flow,
                         blob->len,
                         inet_ntoa(*(struct in_addr*)&extinfo->remoteip_));
*/
   if( blob->len < 2*vrpc::protocol::PKG_LENGTH_LEN )
   {
       base->log_.LOG_P_PID(LOG_ERROR, "spp_handle_process blob->len is too small: %u\n", blob->len );
       RETURN_VALUE(-1, false);
   }

   const char* p_body = NULL;
   uint32_t body_len = 0;
   vrpc::protocol::head cHead;
   iRet = vrpc::protocol::unpack_head( blob->data, blob->len, cHead, p_body, body_len );
   if( (iRet!=0) || (p_body==NULL) || (body_len==0) )
   {
       base->log_.LOG_P_PID(LOG_ERROR, "spp_handle_process unpack_head error.ret:%d\n", iRet );
       RETURN_VALUE(-1, false);
   }

   g_cService.Reset();

   vrpc::example::QueryReq cReq1;
   vrpc::example::QueryResp cResp1;
   vrpc::example::UpdateReq cReq2;
   vrpc::example::UpdateResp cResp2;
   google::protobuf::Message* pcResp = NULL;
   switch( cHead.cmd() )
   {
           case vrpc::example::QUERYINFO_CMD:
                   iServiceRet = g_cService.QueryInfo( cReq1, cResp1 );
                   pcResp = &cResp1;
                   break;
           case vrpc::example::UPDATEINFO_CMD:
                   iServiceRet = g_cService.UpdateInfo( cReq2, cResp2 );
                   pcResp = &cResp2;
                   break;
           default:
                   base->log_.LOG_P_PID(LOG_ERROR, "spp_handle_process unknow cmd:%u\n", cHead.cmd() );
                   RETURN_VALUE(-1, false);
   }

   if( iServiceRet != 0 )
   {
           //todo 应该要上报统计或者监控
           base->log_.LOG_P_PID(LOG_ERROR, "spp_handle_process service return error.ret:%d err:%s\n",
                           iServiceRet, g_cService.GetErrorMessage() );
   }

   iRet = send_response( cHead, pcResp, commu, flow, base );
   if( iRet != 0 )
   {
           base->log_.LOG_P_PID(LOG_ERROR, "send response error, ret:%d\n", iRet);
           RETURN_VALUE(-1, false);
   }

   if( iServiceRet == 0 )
   {
       RETURN_VALUE(0, true);
   }
   else
   {
       RETURN_VALUE(0, false);
   }
}

#undef RETURN_VALUE

//析构资源（可选实现）
//arg1: 保留参数
//arg2: 服务器容器对象
extern "C" void spp_handle_fini(void* arg1, void* arg2)
{
    //服务器容器对象
    CServerBase* base = (CServerBase*)arg2;

    base->log_.LOG_P_PID(LOG_DEBUG, "spp_handle_fini\n");
}

char *format_time( time_t tm)
{
    static char str_tm[1024];
    struct tm tmm;
    memset(&tmm, 0, sizeof(tmm) );
    localtime_r((time_t *)&tm, &tmm);

    snprintf(str_tm, sizeof(str_tm), "[%04d-%02d-%02d %02d:%02d:%02d]",
             tmm.tm_year + 1900, tmm.tm_mon + 1, tmm.tm_mday,
             tmm.tm_hour, tmm.tm_min, tmm.tm_sec);

    return str_tm;
}

int send_response( const vrpc::protocol::head& cHead,
                   google::protobuf::Message* pcResp,
                   CTCommu* comm, unsigned flow, CServerBase* base )
{
    blob_type response;

    pack_message( cHead, *pcResp, response.data, response.len );

    //send response
    int ret = comm->sendto(flow, &response, (void*)base );
    free( response.data );
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

