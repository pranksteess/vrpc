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
 * @date  2012-11-09 �淶������ע��
 * @brief ��L5���ֵ����������շ���(֧��TCP/UDP)
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


/*��˷�������Ϣ*/
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



/** @brief ���������շ���,֧��TCP/UDP,֧��L5����
 *
 *	��װ�������׽��ֳ��ò���������connect/send/recv/close
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
	/** l5����ṹ */
    QOSREQUEST 	mst_qosreq;
	/** ��������,SHORT_SOCK:������; LONG_SOCK:������ */
    int  		mi_sockflag;
	/** ����Э������, 1:tcp,2:udp */
    int 		mi_prot;
	/** �׽���,������ʱʹ�� */
    int 		mi_sock;
	/** �׽��ֳ�ʱʱ�� */
    int  		mi_timeout;
	/** ��ʶ�Ƿ��Ѿ��������� */
    bool 		mb_reved;
	/** ��־���� */
    int  		mi_loglevel;
	/** ��־Ŀ¼ */
    char 		ms_logpath[CLIB_L5_NORMAL];
    /** ������Ϣ */
    char        ms_err[CLIB_L5_HUG];
	/** �׽��ֻ���map */
    std::map<ClibL5, int> m_mpServer;
	/** ȱʡ��������ַ */
    ClibL5 		mst_default_srv;
	/** ���ݷ���ʱ�� */
    struct timeval mst_sendtime;
	/** ���ݽ���ʱ�� */
    struct timeval mst_recvtime;

	/** @brief д��־ */
	void  _log_req(const char * ap_fmt, ...)
		__attribute__((format(__printf__,2,3)));

	/** @brief ����ָ���ķ����� */
    int _connect_to(const char* as_host, const int ai_port, int& ai_sock);
	/** @brief ��ȡָ�����������׽��� */
    int _getsock(const std::string as_host, const int ai_port, int& ai_sock);

	/** @brief ʹ��poll����׽����Ƿ�ɶ���֧�ֳ�ʱ(ms) */
	int _poll_read(const int ai_sock, const int ai_timeout);
	/** @brief ʹ��poll����׽����Ƿ��д��֧�ֳ�ʱ(ms) */
    int _poll_write(const int ai_sock, const int ai_timeout);
	/** @brief ʹ��select����׽����Ƿ�ɶ���֧�ֳ�ʱ(ms) */
    int _select_read(const int ai_sock, const int ai_timeout);
	/** @brief ʹ��select����׽����Ƿ��д��֧�ֳ�ʱ(ms) */
    int _select_write(const int ai_sock, const int ai_timeout);

	/** @brief ͨ���׽��ַ������� */
    int _sock_send(const int ai_sock, const void* ap_buf, const int ai_len);
	/** @brief ͨ���׽��ֽ������� */
    int _sock_recv(const int ai_sock, void* ap_buf, const int ai_len);
	/** @brief ͨ���׽��ֽ���ָ�����ȵ����� */
	int _sock_recv_len(const int ai_sock, void *ap_buf, const int ai_len);

	/** @brief �رյ�ǰ�׽��� */
	int _sock_close(void);
	/** @brief �رյ�ǰ�׽��� */
	int _sock_cache(int ai_sock);

	/** @brief ��ȡһ�����ݣ�֧��ָ�����з� */
	int _tcp_recv_line( const int ai_sock, char* as_buf, const int ai_len, const char* as_eof );

	/** @brief ����׽�����·�Ƿ��� */
    int _check_link_status(void);

public:
	/**
	 * @brief clib_sock���캯��
	 *
	 * @param[in]   ai_modid    L5 modid
	 * @param[in]   ai_cmdid    L5 cmdid
	 * @param[in]   ai_sockflag ��������,1:������,0:������
	 * @param[in]   ai_timeout	��ʱʱ��
	 * @param[in]   ai_prot  	Э��,1:tcp,2:udp
	 */
    clib_sock_l5( int ai_modid = 0, int ai_cmdid = 0, int ai_sockflag = 0,
    	int  ai_timeout = CLIB_DEFAULT_TIME_OUT, int ai_prot = CLIB_SOCK_L5_TCP );

    /**
	* @brief clib_sock_l5��������
	*/
    ~clib_sock_l5( void );

    /**
     * @brief ��ȡ������Ϣ
     */
    inline const char* get_err_msg()
    {
        return ms_err;
    }

	/**
	 * @brief ��ʼ��modid/cmdid/timeout/protocol/sockflag
	 *
	 * @param[in]   ai_modid    L5 modid
	 * @param[in]   ai_cmdid    L5 cmdid
	 * @param[in]   ai_sockflag ��������,1:������,0:������
	 * @param[in]   ai_timeout	��ʱʱ��
	 * @param[in]   ai_prot  	Э��,1:tcp,2:udp
	 * @return      0:ok, <0:fail.
	 */
    int init( int ai_modid, int ai_cmdid, int ai_sockflag, int  ai_timeout, int ai_prot = CLIB_SOCK_L5_TCP );

	/**
	 * @brief ������־·������־����
	 *
	 * @param[in]   as_path		��־·��
	 * @param[in]   ai_loglevel ��־����
	 * @return      0:ok, <0:fail.
	 */
    int set_log( const char* as_path, const int ai_loglevel );

	/**
	 * @brief ����Ĭ�Ϸ�������ַ
	 *
	 * @param[in]   as_host     Ĭ�Ϸ�����ip��ַ
	 * @param[in]   ai_port  	Ĭ�Ϸ������˿�
	 * @return      0:ok, <0:fail.
	 */
	int set_default_server( const char *as_host, const int ai_port );

	/**
	 * @brief �����׽���
	 *
	 * @return      0:ok, <0:fail.
	 */
    int connect( void );

	/**
     * @brief �ͷ������׽���
     *
     * @return		0: ok, !=0: fail.
     */
    int close( void );

	/**
     * @brief ͨ���׽��ַ�������
     *
     * @param[in]   ap_buf     	��������buff
	 * @param[in]   ai_len  	buff����
	 * @return      �������ݳ���. <0: fail.
	 */
    int send( const void* ap_buf, const int ai_len );

	/**
     * @brief ���׽����н�������
     *
     * @param[io]   ap_buf     	��������buff
	 * @param[in]   ai_len  	buff����
	 * @return      �������ݳ���. <0: fail.
	 */
    int recv( void* ap_buf, const int ai_len );

	/**
     * @brief ���׽����н���ָ�����ȵ�����(�����ܵĶ�)
     *
     * @param[io]   ap_buf     	��������buff
	 * @param[in]   ai_len  	buff����
	 * @return      �������ݳ���. <0: fail.
	 */
    int recv_len( void* ap_buf, const int ai_len );

	/**
     * @brief ���׽����н���һ������
     *
     * @param[io]   ap_buf     	��������buff
	 * @param[in]   ai_len  	buff����
	 * @param[in]   as_eof  	���з�
	 * @return      �������ݳ���. <0: fail.
	 */
    int recv_line( char* as_buf, const int ai_len, const char* as_eof = "\r\n");

	/**
	 * @brief ���ͱ���������ý����L5
	 *
	 * @param[in]   ai_ret     	���ý��
	 * @param[in]   ai_time  	����ʱ��
	 * @param[in]   as_msg  	�����Ϣ
	 * @return      0:ok, <0:fail.
	 */
    int update_result( const int ai_ret, const int ai_time, std::string& as_msg );

	/**
	 * @brief ���ͱ���������ý����L5
	 *
	 * @param[in]   ai_ret     	���ý��
	 * @param[in]   as_msg  	�����Ϣ
	 * @return      0:ok, <0:fail.
	 */
    int update_result( const int ai_ret, std::string& as_msg );

	/**
	 * @brief ��ȡ��ǰ���ӷ�����ip
	 *
	 * @return ���ӷ�����IP(std::string&).
	 */
	const std::string& get_current_ip();

	/**
	 * @brief ��ȡ��ǰ���ӷ�����port
	 *
	 * @return ���ӷ�����port(int).
	 */
	int get_current_port();

    // ��ȡ��ǰ������Ϣ
    const std::string get_current_connect();

    int check_link();
    void dump_srvermap();
}; // class clib_sock_l5.

#endif

