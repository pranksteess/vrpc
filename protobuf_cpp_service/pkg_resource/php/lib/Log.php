<?php
/**
 * Created by JetBrains PhpStorm.
 * User: LayenLin
 * Date: 12-4-1
 * Time: 下午3:28
 * To change this template use File | Settings | File Templates.
 */
class Lib_Log
{
	const LEVEL_ERROR = 0;  /* system is unusable               */
	const LEVEL_INFO = 1;   /* informational                    *///流水日志
    const LEVEL_WARNING = 2;/* warning conditions               */
    const LEVEL_DEBUG = 3;  /* debug-level messages             */
    const LEVEL_TRACE = 4;  /* trace-level messages             */

	//日志相关配置
	const NETWORKLOG_HEAD_LEN = 13;
	const NETWORKLOG_VER = 1;
	const NETWORKLOG_CMD = 100;

	//统计来源
	const NETWORKLOG_LOG_SRC_DEFAULT = 0;

	//appid
	const ID = 400;
	
	public static function log($content,$level = self::LEVEL_INFO)
	{
		echo "$content\n";
		self::NetworkLog($content,$level,self::ID);
		return true;
	}
	
	/**
	 * write net log.
	 * @return <bool> true
	 * $appid 请联系会员功能开发组同学申请
	 */	
	private static function NetworkLog($content, $level, $appid, $src = self::NETWORKLOG_LOG_SRC_DEFAULT)
	{
		$errMsg = '';
		$content = $content."\r\n";
		$contentlen = strlen($content) + 1;
		$pkglen = self::NETWORKLOG_HEAD_LEN + $contentlen;
		$buffer = pack("n6Ca$contentlen", $pkglen, self::NETWORKLOG_VER, self::NETWORKLOG_CMD, $src, 0, $appid, $level, $content);
		$l5_route = self::getServerAddress();
		self::UdpSend($buffer, $pkglen, $errMsg, $l5_route['host_ip'].":".$l5_route['host_port']);
		return true;
	}

	/**
	 * 
	 * @return <bool>
	 */	
	private static function UdpSend($strSend, $iSendLen, &$sErrMsg, $strAddress, $iTimeout = 1)
	{
	    $errno = 0;
	    $errstr = "";
	    $fp = fsockopen('udp://' . $strAddress, $errno, $errstr, $iTimeout);
	    if (!$fp)
	    {
	        $sErrMsg = "ERROR: $errno - $errstr";
	        return false;
	    }
	    stream_set_timeout($fp, $iTimeout);
	    $ret = fwrite($fp, $strSend, $iSendLen);
	    if ($ret != $iSendLen)
	    {
	        $sErrMsg = "fwrite failed. ret:[$ret]";
	        if (isset($stream_info['timed_out']))
	        {
	            $sErrMsg .= ' socket_timed_out';
	        }
	        return false;
	    }
	    fclose($fp);
	    return true;
	}
	
    private static function getServerAddress() 
	{
        $MOD_ID = 57729;
        $CMD_ID = 65536;
	    $l5_req = array(
            'flow' => 0,    //无意义，暂时未用
            'modid' => $MOD_ID,    //modid
            'cmd' => $CMD_ID,    //cmdid
            'host_ip' => '',//执行成功时，为路由ip
            'host_port' => 0//执行成功时，为路由port
        );
        $errMsg = '';
        $ret = -1;
        $ret = tphp_l5sys_getroute($l5_req, 0.2, $errMsg);
        if($ret<=0) {
            $l5_req['host_ip'] = '10.187.18.164';
            $l5_req['host_port'] = 12354;
        }
        return $l5_req;
    }		
}
