// vim: set expandtab tabstop=4 shiftwidth=4 fdm=marker:
//+----------------------------------------------------------------------+
// | Club Library.                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) Tencent Inc. All Rights Reserved.            |
// +----------------------------------------------------------------------+
// | Authors: The Club Dev Team, Tencent Inc.                       |
// +----------------------------------------------------------------------+

/**
 * @file clib_sock_l5.h
 * @version v1.0
 * @author  suwenkuang
 * @date  2012-11-09 规范化代码注释
 * @brief 带L5容灾的网络数据收发类(支持TCP/UDP)
 */


#ifndef _CLIB_SOCK_L5_H_
#define _CLIB_SOCK_L5_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <string>
#include <map>
#include <fcntl.h>
#include "qos_client.h"


const int CLIB_SHORT_SOCK =  0;
const int CLIB_LONG_SOCK =  1;
const int CLIB_DEFAULT_TIME_OUT = 1000;
const int CLIB_L5_ERR_CREAT_SOCKET		= -10000 ;
const int CLIB_L5_ERR_CONNECT			= -10001;
const int CLIB_L5_ERR_NOFOUND_SERVER	= -10002;
const int CLIB_L5_ERR_TIMEOUT			= -10003;
const int CLIB_L5_ERR_SELECT			= -10004;
const int CLIB_L5_ERR_FD_NOT_EXIST		= -10005;
const int CLIB_L5_ERR_GRE_L5_ROUT		= -10006;
const int CLIB_L5_ERR_SOCKET			= -10007;
const int CLIB_L5_ERR_PARAM				= -10008;

const int CLIB_L5_SHORT		= 32;
const int CLIB_L5_Mid		= 128;
const int CLIB_L5_NORMAL	= 256;
const int CLIB_L5_LONG		= 512;
const int CLIB_L5_HUG		= 1024;

/// Socket protocol
#define CLIB_SOCK_L5_TCP       SOCK_STREAM
#define CLIB_SOCK_L5_UDP       SOCK_DGRAM


/*后端服务器信息*/
typedef struct ClibL5
{
    std::string  m_sHost;
    int m_iPort;
}ClibL5;

inline bool operator<(const ClibL5 &stServerL51,const ClibL5 &stServerL52)
{
    if (stServerL51.m_sHost < stServerL52.m_sHost)
    {
        return true;
    }
    else if (stServerL51.m_sHost > stServerL52.m_sHost)
    {
    	return false;
    }
    if(stServerL51.m_iPort <  stServerL52.m_iPort)
    {
    	return true;
    }
    else if(stServerL51.m_iPort > stServerL52.m_iPort)
    {
    	return false;
    }
	return false;
}



/** @brief 网络数据收发类,支持TCP/UDP,支持L5容灾
 *
 *	封装了网络套接字常用操作，包括connect/send/recv/close
 *	@code
 *	#include "clib_sock_l5.h"
 *
 *	#define TEST_MODID		1000
 *	#define TEST_CMDID		2000
 *	#define TEST_DEF_IP		8.8.8.8
 *	#define TEST_DEF_PORT	80
 *
 *	int main()
 *	{
 *		int i_ret = 0;
 *		string s_err = "";
 *		char s_send[256] = "test";
 *		char s_recv[256] = {0};
 *		clib_sock_l5 st_sock;
 *
 *		st_sock.init(TEST_MODID, TEST_CMDID, 1, 1000);
 *   	st_sock.set_default_server(TEST_DEF_IP, TEST_DEF_PORT);
 *   	st_sock.set_log("../log/", 1);
 *
 *		i_ret = st_sock.connect();
 *		if ( 0 != i_ret ) {
 *			st_sock.update_result(1, s_err);
 *			return i_ret;
 *		}
 *
 *		i_ret = st_sock.send(s_send, strlen(s_send), 2);
 *		if ( 0 != i_ret ) {
 *			st_sock.update_result(1, s_err);
 *			return i_ret;
 *		}
 *
 *		i_ret = st_sock.recv(s_recv, sizeof(s_recv), 2);
 *		if ( 0 != i_ret ) {
 *			st_sock.update_result(1, s_err);
 *			return i_ret;
 *		}
 *		st_sock.close();
 *		st_sock.update_result(0, s_err);
 *
 *		return 0;
 *	}
 *	@endcode
 */
class clib_sock_l5
{
private:
	/** l5请求结构 */
    QOSREQUEST 	mst_qosreq;
	/** 连接类型,SHORT_SOCK:短连接; LONG_SOCK:长连接 */
    int  		mi_sockflag;
	/** 连接协议类型, 1:tcp,2:udp */
    int 		mi_prot;
	/** 套接字,短连接时使用 */
    int 		mi_sock;
	/** 套接字超时时间 */
    int  		mi_timeout;
	/** 标识是否已经发送数据 */
    bool 		mb_reved;
	/** 日志级别 */
    int  		mi_loglevel;
	/** 日志目录 */
    char 		ms_logpath[CLIB_L5_NORMAL];
    /** 错误信息 */
    char        ms_err[CLIB_L5_HUG];
	/** 套接字缓存map */
    std::map<ClibL5, int> m_mpServer;
	/** 缺省服务器地址 */
    ClibL5 		mst_default_srv;
	/** 数据发送时间 */
    struct timeval mst_sendtime;
	/** 数据接收时间 */
    struct timeval mst_recvtime;

	/** @brief 写日志 */
	void  _log_req(const char * ap_fmt, ...)
		__attribute__((format(__printf__,2,3)));

	/** @brief 链接指定的服务器 */
    int _connect_to(const char* as_host, const int ai_port, int& ai_sock);
	/** @brief 获取指定服务器的套接字 */
    int _getsock(const std::string as_host, const int ai_port, int& ai_sock);

	/** @brief 使用poll检查套接字是否可读，支持超时(ms) */
	int _poll_read(const int ai_sock, const int ai_timeout);
	/** @brief 使用poll检查套接字是否可写，支持超时(ms) */
    int _poll_write(const int ai_sock, const int ai_timeout);
	/** @brief 使用select检查套接字是否可读，支持超时(ms) */
    int _select_read(const int ai_sock, const int ai_timeout);
	/** @brief 使用select检查套接字是否可写，支持超时(ms) */
    int _select_write(const int ai_sock, const int ai_timeout);

	/** @brief 通过套接字发送数据 */
    int _sock_send(const int ai_sock, const void* ap_buf, const int ai_len);
	/** @brief 通过套接字接收数据 */
    int _sock_recv(const int ai_sock, void* ap_buf, const int ai_len);
	/** @brief 通过套接字接收指定长度的数据 */
	int _sock_recv_len(const int ai_sock, void *ap_buf, const int ai_len);

	/** @brief 关闭当前套接字 */
	int _sock_close(void);
	/** @brief 关闭当前套接字 */
	int _sock_cache(int ai_sock);

	/** @brief 收取一行数据，支持指定换行符 */
	int _tcp_recv_line( const int ai_sock, char* as_buf, const int ai_len, const char* as_eof );

	/** @brief 检查套接字链路是否存活 */
    int _check_link_status(void);

public:
	/**
	 * @brief clib_sock构造函数
	 *
	 * @param[in]   ai_modid    L5 modid
	 * @param[in]   ai_cmdid    L5 cmdid
	 * @param[in]   ai_sockflag 连接类型,1:长链接,0:短链接
	 * @param[in]   ai_timeout	超时时间
	 * @param[in]   ai_prot  	协议,1:tcp,2:udp
	 */
    clib_sock_l5( int ai_modid = 0, int ai_cmdid = 0, int ai_sockflag = 0,
    	int  ai_timeout = CLIB_DEFAULT_TIME_OUT, int ai_prot = CLIB_SOCK_L5_TCP );

    /**
	* @brief clib_sock_l5析构函数
	*/
    ~clib_sock_l5( void );

    /**
     * @brief 获取出错信息
     */
    inline const char* get_err_msg()
    {
        return ms_err;
    }

	/**
	 * @brief 初始化modid/cmdid/timeout/protocol/sockflag
	 *
	 * @param[in]   ai_modid    L5 modid
	 * @param[in]   ai_cmdid    L5 cmdid
	 * @param[in]   ai_sockflag 连接类型,1:长链接,0:短链接
	 * @param[in]   ai_timeout	超时时间
	 * @param[in]   ai_prot  	协议,1:tcp,2:udp
	 * @return      0:ok, <0:fail.
	 */
    int init( int ai_modid, int ai_cmdid, int ai_sockflag, int  ai_timeout, int ai_prot = CLIB_SOCK_L5_TCP );

	/**
	 * @brief 设置日志路径和日志级别
	 *
	 * @param[in]   as_path		日志路径
	 * @param[in]   ai_loglevel 日志级别
	 * @return      0:ok, <0:fail.
	 */
    int set_log( const char* as_path, const int ai_loglevel );

	/**
	 * @brief 设置默认服务器地址
	 *
	 * @param[in]   as_host     默认服务器ip地址
	 * @param[in]   ai_port  	默认服务器端口
	 * @return      0:ok, <0:fail.
	 */
	int set_default_server( const char *as_host, const int ai_port );

	/**
	 * @brief 链接套接字
	 *
	 * @return      0:ok, <0:fail.
	 */
    int connect( void );

	/**
     * @brief 释放所有套接字
     *
     * @return		0: ok, !=0: fail.
     */
    int close( void );

	/**
     * @brief 通过套接字发送数据
     *
     * @param[in]   ap_buf     	发送数据buff
	 * @param[in]   ai_len  	buff长度
	 * @return      发送数据长度. <0: fail.
	 */
    int send( const void* ap_buf, const int ai_len );

	/**
     * @brief 从套接字中接收数据
     *
     * @param[io]   ap_buf     	接收数据buff
	 * @param[in]   ai_len  	buff长度
	 * @return      接收数据长度. <0: fail.
	 */
    int recv( void* ap_buf, const int ai_len );

	/**
     * @brief 从套接字中接收指定长度的数据(尽可能的读)
     *
     * @param[io]   ap_buf     	接收数据buff
	 * @param[in]   ai_len  	buff长度
	 * @return      接收数据长度. <0: fail.
	 */
    int recv_len( void* ap_buf, const int ai_len );

	/**
     * @brief 从套接字中接收一行数据
     *
     * @param[io]   ap_buf     	接收数据buff
	 * @param[in]   ai_len  	buff长度
	 * @param[in]   as_eof  	换行符
	 * @return      接收数据长度. <0: fail.
	 */
    int recv_line( char* as_buf, const int ai_len, const char* as_eof = "\r\n");

	/**
	 * @brief 发送本次网络调用结果给L5
	 *
	 * @param[in]   ai_ret     	调用结果
	 * @param[in]   ai_time  	调用时间
	 * @param[in]   as_msg  	结果信息
	 * @return      0:ok, <0:fail.
	 */
    int update_result( const int ai_ret, const int ai_time, std::string& as_msg );

	/**
	 * @brief 发送本次网络调用结果给L5
	 *
	 * @param[in]   ai_ret     	调用结果
	 * @param[in]   as_msg  	结果信息
	 * @return      0:ok, <0:fail.
	 */
    int update_result( const int ai_ret, std::string& as_msg );

	/**
	 * @brief 获取当前连接服务器ip
	 *
	 * @return 连接服务器IP(std::string&).
	 */
	const std::string& get_current_ip();

	/**
	 * @brief 获取当前连接服务器port
	 *
	 * @return 连接服务器port(int).
	 */
	int get_current_port();

    // 获取当前链接信息
    const std::string get_current_connect();

    int check_link();
    void dump_srvermap();
}; // class clib_sock_l5.

#endif

