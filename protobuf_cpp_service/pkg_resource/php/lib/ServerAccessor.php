<?php

//server调用封装
class Lib_ServerAccessor
{
	public static function CallServer($server, $bpHead, $pbBody, &$pbRecv, &$errMsg = "", $mcId = null, $timeout = 3)
	{
		$l5req = null;
		$host = $server['ip'];
		$port = $server['port'];
		$starttime = microtime();

		if (isset($server["modid"]) && isset($server["cmdid"])) 
		{
			$l5req = Lib_L5::getRoute($server["modid"], $server["cmdid"], $server['ip'], $server['port']);
			$host = $l5req['host_ip'];
			$port = $l5req['host_port'];
		}
		$sock = new tphp_socket($host, $port, $timeout);

		/** 生成请求字符串 */
		$str_head = $bpHead->SerializeToString();
		$str_body = $pbBody->SerializeToString();
		$str_head_len = pack("N",strlen($str_head));
		$str_msg_len = pack("N",strlen($str_head_len.$str_head.$str_body)+4);
		
		$msg_str = $str_msg_len.$str_head_len.$str_head.$str_body;

		if (!is_null($mcId)) 
		{
			Lib_MCMonitor::begin('CallServer', $mcId, $server['mcmModuleId'], $port, $host);
		}

		/** 连接 */
		$ret = $sock->connect();

		if ($ret != 0) 
		{
			if (!is_null($mcId)) 
			{
				Lib_MCMonitor::end(Lib_MCMonitor::MCMONITOR_RESULT_NETEXCEPTION, $ret, __LINE__, __FILE__, getlastmod());
			}

			Lib_Log::log("callserver[$host:$port] send[$send_str] err[connect failed] msg[{$sock->get_errmsg()}] time[".Exp_Util_Tool::getcosttime($starttime)."ms]", Lib_Log::LEVEL_ERROR);
			$sock->close();

			if (!is_null($l5req)) 
			{
				Lib_L5::update($l5req, $starttime, -1);
			}

			return false;
		}

		/** 发送 */
		$ret = $sock->write_line($msg_str);
		if ($ret == false) 
		{
			if (!is_null($mcId)) 
			{
				Lib_MCMonitor::end(Lib_MCMonitor::MCMONITOR_RESULT_IOEXCEPTION, 0, __LINE__, __FILE__, getlastmod());
			}
			Lib_Log::log("callserver[$host:$port] send[$send_str] err[write_line failed] msg[{$sock->get_errmsg()}] time[".Exp_Util_Tool::getcosttime($starttime)."ms]", Lib_Log::LEVEL_ERROR);
			$sock->close();

			if (!is_null($l5req)) 
			{
				Lib_L5::update($l5req, $starttime, -1);
			}

			return false;
		}

		/** 接收 */
		$str_head = fread($sock->sock, 4);
		$head = unpack("N", $str_head);				
		$ret = $str_head.fread($sock->sock, $head[1]-4);
		if ($ret == false) 
		{
			Lib_Log::log("callserver[$host:$port] send[$msg_str] err[read_line failed] msg[{$sock->get_errmsg()}]", Lib_Log::LEVEL_ERROR);		
			$sock->close();
			if (!is_null($l5req)) 
			{
				Lib_L5::update($l5req, $starttime, -1);
			}
			return false;
		}
		if (!is_null($mcId)) 
		{
			Lib_MCMonitor::end(Lib_MCMonitor::MCMONITOR_RESULT_SUCCESS, 0, __LINE__, __FILE__, getlastmod());
		}
		$sock->close();
		//$ret = str_replace("\r\n", "", $ret);
		
		$resp_len = unpack("N",substr($ret,0,4));
		$head_len = unpack("N",substr($ret,4,4));
		
		$str_pbRecv = substr($ret,8+$head_len[1],$resp_len[1]-$head_len[1]-8);
		$pbRecv->parseFromString($str_pbRecv);

		if (!is_null($l5req)) 
		{
			Lib_L5::update($l5req, $starttime, 0);
		}
		return true;
	}
}
