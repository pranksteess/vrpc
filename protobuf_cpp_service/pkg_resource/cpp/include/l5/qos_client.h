#ifndef _QOS_AGENT_H_
#define _QOS_AGENT_H_
#include <netdb.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <errno.h> 
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include<stdint.h>
#include <string>
#include <vector>
extern int h_errno;
using namespace std;

#define QOS_TM_MAX			20
//ApiQosReqCheck�ķ�����
enum _QOS_RTN
{
	QOS_RTN_OK	   		= 0x00,
	QOS_RTN_ACCEPT,					//����
	QOS_RTN_LROUTE,
	QOS_RTN_TROUTE,
	QOS_RTN_STATIC_ROUTE,  // return route from local static route
	QOS_RTN_INITED,
	QOS_RTN_OVERLOAD	= -10000,	//����
	QOS_RTN_TIMEOUT,				//��ʱ
	QOS_RTN_SYSERR,					//ϵͳ����
	QOS_RTN_SENDERR,
	QOS_RTN_RECVERR,
	QOS_MSG_INCOMPLETE,
	QOS_CMD_ERROR,
	QOS_MSG_CMD_ERROR,
	QOS_INIT_CALLERID_ERROR,
	QOS_RTN_PARAM_ERROR
};

//QOS����ģʽ
enum _QOS_RUN_TYPE
{
	QOS_TYPE_REQ = 0x00,		//���������Ʒ�ʽ
	QOS_TYPE_LIST				//���������Ʒ�ʽ
};

struct QOSREQUESTTMEXTtag;
//���η��ʵĻ�����Ϣ
struct QOSREQUESTtag
{
	int			_flow;
	int			_modid;		//����ģ�����
	int			_cmd;		//�����ӿڱ���
	string		_host_ip;	//��������IP
	unsigned short		_host_port;	//��������PORT
	int _pre;     
   	
	QOSREQUESTtag(const QOSREQUESTTMEXTtag &route);
	QOSREQUESTtag():_flow(0),_modid(0),_cmd(0),_host_port(0),_pre(0){}
	/*
	QOSREQUESTtag(QOSREQUESTTMEXTtag &route)
	{
		_flow = route._flow;
		_modid = route._modid;
		_cmd = route._cmd;
		_host_ip = route._host_ip;
		_host_port = route._host_port;
		_pre = route._pre;
	}*/	 
};

typedef struct QOSREQUESTtag QOSREQUEST;

//������ʱ����Ϣ�Ľӿ�
struct QOSREQUESTTMEXTtag
{
	int			_flow;
	int			_modid;		//����ģ�����
	int			_cmd;		//�����ӿڱ���
	string		_host_ip;	//��������IP
	unsigned short		_host_port;	//��������PORT
	int _pre;        
	int _delay;        

	QOSREQUESTTMEXTtag( ):_flow(0),_modid(0),_cmd(0),_host_port(0),_pre(0),_delay(0){}
	QOSREQUESTTMEXTtag( const QOSREQUEST & route);
/*	QOSREQUESTTMEXTtag( QOSREQUEST & route)
	{
		_flow = route._flow;
		_modid = route._modid;
		_cmd = route._cmd;
		_host_ip = route._host_ip;
		_host_port = route._host_port;
		_pre = route._pre;
	}*/	
};
typedef struct QOSREQUESTTMEXTtag QOSREQUEST_TMEXT;

typedef struct QOSREQUESTMTTCEXTtag
{
	int32_t			_modid;		//����ģ�����
	int32_t			_cmdid;
	int64_t			_key;		//�����ӿڱ���
	int32_t			_funid;
	string		_host_ip;	//��������IP
	unsigned short		_host_port;	//��������PORT
}QOSREQUEST_MTTCEXT;

typedef struct QOSREQUESTMTTC_TMEXTtag
{
	int32_t			_modid;		//����ģ�����
	int32_t			_cmdid;
	int64_t			_key;		//�����ӿڱ���
	int32_t			_funid;
	string		_host_ip;	//��������IP
	unsigned short		_host_port;	//��������PORT
	int32_t			_delay;    //·�ɷ���ʱ��
}QOSREQUEST_MTTCEXT_TM;


//���η��ʵĻ�����Ϣ
typedef struct CALLERQOSREQUESTtag
{
	int			_flow;
	int               _callermod;
	int               _callercmd;
	int			_calledmodid;		//����ģ�����
	int			_calledcmd;		//�����ӿڱ���
	string		_host_ip;	//��������IP
	unsigned short		_host_port;	//��������PORT
}CALLERQOSREQUEST;

typedef struct CALLERQOSREQUESTMTTCEXTtag
{
	int               _callermod;
	int               _callercmd;
	int				_calledmodid;		//����ģ�����
	int				_calledcmdid;
	int64_t			_calledkey;		//�����ӿڱ���
	int32_t			_funid;
	string		_host_ip;	//��������IP
	unsigned short		_host_port;	//��������PORT
}CALLERQOSREQUEST_MTTCEXT;


//QOS���������Ƶ�������Ϣ
struct QOSREQCFG
{
	int			_req_max;			//���������Ƶ����ֵ
	int			_req_min;			//���������Ƶ���Сֵ
	float		_req_err_min;		//��������ֵ[С�����ֵ��Ϊ���޴�]
	float		_req_extend_rate;	//�޴����ʱ��ķ�ֵ������
	
	QOSREQCFG()
	{
		_req_max = 0;
		_req_min = 0;
		_req_err_min = 0.0;
		_req_extend_rate = 0.0;
	};
};

//QOS���������Ƶ�������Ϣ
struct QOSLISTCFG
{
	int			_list_max;			//���������Ƶ����ֵ
	int			_list_min;			//���������Ƶ���Сֵ
	float		_list_err_min;		//���������ֵ[С�����ֵ��Ϊ���޴�]
	float		_list_extend_rate;	//�����޴����ʱ��ķ�ֵ������
	
	QOSLISTCFG()
	{
		_list_max = 0;
		_list_min = 0;
		_list_err_min = 0.0;
		_list_extend_rate = 0.0;
	};
};

//QOS���ʰ�ʱ��ֶε�������Ϣ
struct QOSTMCFG
{
	int		_tm_cfg_count;					//���õĸ���
	int		_begin_usec[QOS_TM_MAX];		//����ʱ��ο�ʼʱ��
	int		_end_usec[QOS_TM_MAX];			//����ʱ��ν���ʱ��
	int		_ret[QOS_TM_MAX];				//����ֵ[0:�ɹ� -1:ʧ��]
	
	QOSTMCFG()
	{
		_tm_cfg_count = 0;
		for ( int i=0;i<QOS_TM_MAX;i++ )
		{
			_begin_usec[i] = 0;
			_end_usec[i] = 0;
			_ret[i] = 0;
		};
	};
};

typedef struct Route_CycTm
{
	int _modid;
	int _cmd;
	int tm;
	int minreq;
	int maxreq;
	float ext;
	float min_err;

} ROUTE_CYCTM;
/*
//����QOS����������
int ApiQosReqCfg(QOSREQUEST& qos_req,QOSREQCFG& qos_req_cfg,float time_out,string& err_msg);
//QOS����������ΪϵͳĬ��
int ApiQosReqCfgDel(QOSREQUEST& qos_req,float time_out,string& err_msg);
//����QOS����������
int ApiQosListCfg(QOSREQUEST& qos_req,QOSLISTCFG& qos_list_cfg,float time_out,string& err_msg);
//QOS����������ΪϵͳĬ��
int ApiQosListCfgDel(QOSREQUEST& qos_req,float time_out,string& err_msg);
//��ʼ����ʱʱ������
int ApiQosTmCfg(QOSREQUEST& qos_req,QOSTMCFG& qos_tm_cfg,float time_out,string& err_msg);
//��ʱʱ������ΪϵͳĬ��
int ApiQosTmCfgDel(QOSREQUEST& qos_req,float time_out,string& err_msg);
//����QOS����ģʽ
int ApiQosTypeCfg(QOSREQUEST& qos_req,_QOS_RUN_TYPE qos_type,float time_out,string& err_msg);
//����QOS����ģʽΪϵͳĬ��
int ApiQosTypeCfgDel(QOSREQUEST& qos_req,float time_out,string& err_msg);

//�����ʵ�SERVER�Ƿ����
int ApiQosReqCheck(QOSREQUEST& qos_req,float time_out,string& err_msg);


//���·�����Ϣ
int ApiQosReqUpdate(QOSREQUEST& qos_req,int ret,int usetime_msec,string& err_msg);


int ApiRouteTmCfg(QOSREQUEST& qos_req,QOSTMCFG& qos_tm_cfg,float time_out,string& err_msg);

int ApiRouteTmCfgDel(QOSREQUEST& qos_req,float time_out,string& err_msg);
*/



/*
 *   Function: ApiGetRoute
 *   Args:
 *       modid:      modid needed to be inited
 *       cmdid:      cmdid needed to be inited
 *       time_out:   time limit to fetch route    
 *       err_msg:    error messange when return value<0
 *   Ret:
 *       0 for route(modid,cmdid) alreadly in l5agent 
 *       4 for alreadly notify dnsagent to fetch route(modid,cmdid) 
 *       <0 for errors,while err_msg will be filled 
 */
int ApiInitRoute(int modid, int cmdid, float time_out, string& err_msg);

// added by ericsha, Get/Set L5 Port.
void ApiGetL5Port(int &port);
void ApiSetL5Port(int port);
int ApiSetUpdateInterval(int updateCycle);

int ApiRouteNodeCfg(QOSREQUEST& qos_req,QOSREQCFG& qos_req_cfg,float time_out,string& err_msg);

int ApiRouteNodeCfgDel(QOSREQUEST& qos_req,float time_out,string& err_msg);


int ApiOutRoutTable(float time_out,string& err_msg);




int ApiAddRouteNode(QOSREQUEST& qos_req,float time_out,string& err_msg);

/*
	Function: ApiGetRoute
	Args:
		qos_req:	feedback info  reported to L5_agent,including modid, cmdid
		time_out:	time limit to fetch route,the actrual time limit is 1s* time_out	
		tm_val:		time stamp transfered to api,in order to reduce gettimeofday,default as NULL
		err_msg:	error messange when return value<0
	Ret:
		0 for OK
		<0 for errors,while err_msg will be filled 
*/

//·��
int ApiGetRoute(QOSREQUEST& qos_req,float time_out,string& err_msg,struct timeval* tm_val=NULL);
int ApiGetRoute(QOSREQUEST_MTTCEXT& qos_req,float time_out,string& err_msg,struct timeval* tm_val=NULL);
int ApiGetRoute(QOSREQUEST_TMEXT& qos_req,float time_out,string& err_msg,struct timeval* tm_val=NULL);
int ApiGetRoute(QOSREQUEST_MTTCEXT_TM& qos_req,float time_out,string& err_msg,struct timeval* tm_val=NULL);

int ApiAntiParallelGetRoute(QOSREQUEST_MTTCEXT& qos_req,float time_out,string& err_msg,struct timeval* tm_val=NULL);
//int ApiGetRoute(QOSREQUEST& qos_req,float time_out,string& err_msg);

int ApiRouteNodeDel(QOSREQUEST& qos_req,float time_out,string& err_msg);

/*
	Function: ApiRouteResultUpdate
	Args:
		qos_req:	feedback info  reported to L5_agent,including modid, cmdid
		ret:		status report to L5_agent,0 for nomal ,<0 for abnormal
		tm_val:		time stamp transfered to api,in order to reduce gettimeofday,default as NULL
		err_msg:	error messange when return value<0
	Ret:
		0 for OK
		<0 for errors,while err_msg will be filled 
*/

int ApiRouteResultUpdate(QOSREQUEST& qos_req,int ret, int usetime_usec,string& err_msg,struct timeval* tm_val=NULL);
int ApiRouteResultUpdate(QOSREQUEST_MTTCEXT& qos_req,int ret, int usetime_usec,string& err_msg,struct timeval* tm_val=NULL);
//int ApiRouteResultUpdate(QOSREQUEST& qos_req,int ret, int usetime_usec,string& err_msg);
int ApiRouteResultUpdate(QOSREQUEST_TMEXT& qos_req,int ret,int usetime_usec,string& err_msg,struct timeval* tm_val=NULL);
int ApiAntiParallelUpdate(QOSREQUEST_MTTCEXT& qos_req,int ret,int usetime_usec,string& err_msg,struct timeval* tm_val=NULL);
int ApiRouteResultUpdate(QOSREQUEST_MTTCEXT_TM& qos_req,int ret,int usetime_usec,string& err_msg,struct timeval* tm_val=NULL);


int ApiCallerRouteResultUpdate(CALLERQOSREQUEST& qos_req,int ret, int usetime_usec,string& err_msg,struct timeval* tm_val=NULL);
int ApiCallerRouteResultUpdate(CALLERQOSREQUEST_MTTCEXT& qos_req,int ret, int usetime_usec,string& err_msg,struct timeval* tm_val=NULL);
//int ApiCallerRouteResultUpdate(CALLERQOSREQUEST& qos_req,int ret, int usetime_usec,string& err_msg);



int ApiRouteCofCycTm(const ROUTE_CYCTM& qos_req);

void ApiSetL5Time(int* tm,struct timeval* tm_val);


/*
int ApiSendRouteReq(int sockid,const QOSREQUEST& qos_req,string& err_msg);
int ApiRecvRouteRsp(int sockid, QOSREQUEST& qos_req,string& err_msg);
int ApiRecvRouteRspTimer(int sockid, QOSREQUEST& qos_req, float time_out, string& err_msg);
*/

/*
int ApiSendRouteReqByUnix(int sockid,const QOSREQUEST& qos_req,string& err_msg);
int ApiRecvRouteRspTimerByUnix(int sockid, QOSREQUEST& qos_req, float time_out, string& err_msg);
int ApiRecvRouteRspByUnix(int sockid, QOSREQUEST& qos_req,  string& err_msg);
int ApiRouteResultUpdateByUnix(int sockid, QOSREQUEST& qos_req,int ret,int usetime_usec,string& err_msg);
int ApiRecvMtRouteRsp(QOSREQUEST& qos_req,char* _recv_buf,int& offset, int _recv_buf_len,  string& err_msg);

int connect_l5agent(bool block);*/

static inline void*  map_file (const char* filename, int size)
{
	int fd = ::open(filename, O_RDWR|O_CREAT, 0666);
	void *map = NULL;

	if(fd >= 0)
	{
		if(size > 0)
			ftruncate(fd, size);
		else
			size = lseek(fd, 0L, SEEK_END);

		if(size > 0)
			map = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
		::close(fd);
	}
	else if(size > 0)
	{
		map = mmap(0, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS, -1, 0);
	}

	if(map == MAP_FAILED)
	{
		map = NULL;
	}
	return map;
}


#endif
