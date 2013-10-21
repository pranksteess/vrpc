<?php

class Lib_L5
{

	public static function getRoute($modid, $cmdid, $host = null, $port = null)
	{
		Lib_MCMonitor::begin('tphp_l5_getroute', 106000500, 206000233);

		if (!extension_loaded('tphp_l5sys')) {
			if (!ini_get('safe_mode') && ini_get('enable_dl')) {
				if (!@dl('tphp_l5sys.so')) {
					Lib_MCMonitor::end(Lib_MCMonitor::MCMONITOR_RESULT_IOEXCEPTION, 0, __LINE__, __FILE__, getlastmod());
					trigger_error('load tphp_l5sys.so error');
					return NULL;
				}
			} else {
				Lib_MCMonitor::end(Lib_MCMonitor::MCMONITOR_RESULT_IOEXCEPTION, 0, __LINE__, __FILE__, getlastmod());
				trigger_error('dl is not enabled!');
				return NULL;
			}
		}

		$errmsg = '';
		$l5req = array('flow' => 0, 'modid' => $modid, 'cmd' => $cmdid, 'host_ip' => '', 'host_port' => 0);
		$ret = tphp_l5sys_getroute($l5req, 0.2, $errmsg);

		if ($ret < 0) {
			//get route failed
			Lib_MCMonitor::end(Lib_MCMonitor::MCMONITOR_RESULT_FAILURE, $ret, __LINE__, __FILE__, getlastmod());
			trigger_error('tphp_l5sys_getroute error ret:' . $ret . ' errmsg:' . trim($errmsg) . ' modid:' . $l5req['modid'] . ' cmd:' . $l5req['cmd'], E_USER_WARNING);
			return array('host_ip' => $host, 'host_port' => $port);
		}

		Lib_MCMonitor::end(Lib_MCMonitor::MCMONITOR_RESULT_SUCCESS, 0, __LINE__, __FILE__, getlastmod());
		return ($l5req);
	}

	/**
	 * Update l5 info.
	 * @param   int     $interface_ret        MSS return.
	 *
	 * @return                      1: have; 0: none; -1: fail.
	 */
	public static function update($l5req, $start_time, $interface_ret)
	{
		Lib_MCMonitor::begin('tphp_l5_update', 106000501, 206000233);

		if (!extension_loaded('tphp_l5sys')) {
			Lib_MCMonitor::end(Lib_MCMonitor::MCMONITOR_RESULT_IOEXCEPTION, 0, __LINE__, __FILE__, getlastmod());
			return -1;
		}
		if ($start_time == '') {
			Lib_MCMonitor::end(Lib_MCMonitor::MCMONITOR_RESULT_IOEXCEPTION, 0, __LINE__, __FILE__, getlastmod());
			return -2;
		}
		list($usec_start, $sec_start) = explode(' ', $start_time);
		list($usec_end, $sec_end) = explode(' ', microtime());
		$usetime_usec = ($sec_end - $sec_start) * 1000000 + ($usec_end - $usec_start);

		$errmsg = '';
		$ret = tphp_l5sys_route_result_update($l5req, $interface_ret, $usetime_usec, $errmsg);

		if ($ret < 0) {
			//update failed
			Lib_MCMonitor::end(Lib_MCMonitor::MCMONITOR_RESULT_FAILURE, 0, __LINE__, __FILE__, getlastmod());
			trigger_error('tphp_l5sys_route_result_update error ret:' . $ret . ' errmsg:' . trim($errmsg) . ' host:' . $l5req['host_ip'] . ' port:' . $l5req['host_port'] . ' use:' . $usetime_usec, E_USER_WARNING);
		}
		Lib_MCMonitor::end(Lib_MCMonitor::MCMONITOR_RESULT_SUCCESS, 0, __LINE__, __FILE__, getlastmod());
		return ($ret);
	}

}