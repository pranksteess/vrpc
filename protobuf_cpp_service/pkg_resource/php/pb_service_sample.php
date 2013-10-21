<?php
define('ROOT_PATH', dirname(__FILE__).'/');
define('MSG_PATH', ROOT_PATH.'message/');
define('LIB_PATH',ROOT_PATH.'lib/');
require_once 'tphplib.inc.php';
require_once MSG_PATH.'pb_message.php';
require_once ROOT_PATH.'pb_proto_test4ryan.php';
require_once LIB_PATH.'L5.php';
require_once LIB_PATH.'Log.php';
require_once LIB_PATH.'MCMonitor.php';
require_once LIB_PATH.'ServerAccessor.php';
require_once LIB_PATH.'pb_proto_head.php';

class PricingService
{
	public static $server;
	public static function set_server($svrinfo)
	{
		PricingService::$server = $svrinfo;
	}
	public static function query($req)
	{
		$pbRecv = new RspGetItemInfo();
		
		$bpHead = new Lib_Head();
		$bpHead->set_ver(1);
		$bpHead->set_cmd(0);
		$bpHead->set_seq(0);
		
		if(Lib_ServerAccessor::CallServer(PricingService::$server, $bpHead, $req, &$pbRecv))
			return $pbRecv;
		return false;
	}
	public static function pricing($req)
	{
		$pbRecv = new RspPricing();
		
		$bpHead = new Lib_Head();
		$bpHead->set_ver(1);
		$bpHead->set_cmd(1);
		$bpHead->set_seq(0);
		
		if(Lib_ServerAccessor::CallServer(PricingService::$server, $bpHead, $req, &$pbRecv))
			return $pbRecv;
		return false;
	}
}
?>