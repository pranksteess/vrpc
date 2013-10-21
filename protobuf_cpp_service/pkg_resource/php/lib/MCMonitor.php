<?php

class Lib_MCMonitor
{
	const MCMONITOR_LOG_ID = 2045000016;
	const MCMONITOR_LOG_LEVEL = 8;
	const MCMONITOR_LOG_CALLERMODULEID = 206000226;

	const MCMONITOR_RESULT_SUCCESS = 0;
	const MCMONITOR_RESULT_FAILURE = 1;
	const MCMONITOR_RESULT_LOGICFAILURE = 2;
	const MCMONITOR_RESULT_DBEXCEPTION = 3;
	const MCMONITOR_RESULT_NETEXCEPTION = 4;
	const MCMONITOR_RESULT_IOEXCEPTION = 5;
	const MCMONITOR_RESULT_OTHEREXCEPTION = 6;


	protected static $singleton = NULL;

	protected static $logId = self::MCMONITOR_LOG_ID;

	protected static $logLevel = self::MCMONITOR_LOG_LEVEL;

	protected static $callerModuleID = self::MCMONITOR_LOG_CALLERMODULEID;

	/**
	 *  获取单件实例
	 *
	 * @return {ModuleCalling} 实例
	 */
	public static function getInstance()
	{
		if (self::$singleton === NULL) {
			$callerPort = defined('MCMONITOR_LOG_CALLERPORT') ? MCMONITOR_LOG_CALLERPORT : intval(self::getEnvironment('SERVER_PORT', 80));
			$callerIP = defined('MCMONITOR_LOG_CALLERIP') ? tphp_ip2long(MCMONITOR_LOG_CALLERIP) : false;
			$callerIP = ($callerIP) ? $callerIP : intval(tphp_ip2long(self::getEnvironment('SERVER_ADDR', '127.0.0.1')));

			$logInfo = array('logId' => self::$logId, 'callerModuleID' => self::$callerModuleID, 'callerPort' => $callerPort, 'callerIP' => $callerIP,);

			self::$singleton = new self($logInfo);
		}
		return self::$singleton;
	}

	/**
	 *  获取环境变量
	 *
	 * @param {string} $name 变量名
	 * @param {mixed} $default 默认值
	 * @return {mixed} 变量值
	 */
	protected static function getEnvironment($name, $default = '')
	{
		if (array_key_exists($name, $_SERVER)) {
			return $_SERVER[$name];
		} else {
			return $default;
		}
	}

	/**
	 *  上报模调日志
	 *
	 * @param {array} $logInfo 日志
	 * @return {int} 上报结果
	 */
	public static function log($logInfo)
	{
		$log = new tmsglog('TUAN_ML_LOG');

		return $log->msgprintf($logInfo['logId'], '%d%d%d%d%d%d%d%s%d%s%s%d%d%d%d%d%d%d%d%s%s%s%s%s', $logInfo['callerModuleID'], $logInfo['calledModuleID'], $logInfo['calledInterfaceID'], $logInfo['callerIP'], $logInfo['calledIP'], $logInfo['callerPort'], $logInfo['calledPort'], $logInfo['callerFileName'], $logInfo['callerFileLine'], $logInfo['callerFileLastModTime'], $logInfo['operateType'], $logInfo['callReturn'], $logInfo['callResult'], $logInfo['totalTime'], -1, -1, -1, -1, -1, '', '', '', '', '');
	}

	/**
	 *  上报错误日志
	 *
	 * @param {array} $logInfo 日志
	 */
	public static function logError($logInfo)
	{
		Lib_Log::log($logInfo, Lib_Log::LEVEL_WARNING);
	}

	/**
	 *  记录开始时间
	 *
	 * @param {string?} $operateType 操作类型
	 * @param {int} $calledInterfaceID 被调接口ID
	 * @param {int} $calledModuleID 被调模块ID
	 * @param {int} $calledPort 被调端口
	 * @param {int} $calledIP 被调IP
	 * @param {int} $logLevel 日志等级
	 * @return {int} 日志索引
	 */
	public static function begin($operateType, $calledInterfaceID, $calledModuleID, $calledPort = NULL, $calledIP = NULL, $logLevel = 0)
	{
		$beginTime = 0;

		if (self::$logLevel > $logLevel) {
			$beginTime = microtime(true);
		}

		return self::getInstance()->insert(array('beginTime' => $beginTime, 'logLevel' => $logLevel, 'operateType' => $operateType, 'calledInterfaceID' => $calledInterfaceID, 'calledModuleID' => $calledModuleID, 'calledPort' => is_int($calledPort) ? $calledPort : intval(self::getEnvironment('SERVER_PORT', 80)), 'calledIP' => intval(tphp_ip2long(($calledIP) ? $calledIP : self::getEnvironment('SERVER_ADDR', '127.0.0.1')))));
	}

	/**
	 *  记录结束时间
	 *
	 * @param {int} $callResult 调用结果
	 * @param {int} $callReturn 调用返回值
	 * @param {int} $callerFileLine 主调文件行号
	 * @param {string} $callerFileName 主调文件名
	 * @param {int} $callerFileLastModTime 主调模块最后修改时间
	 * @param {int} $logLevel 日志等级
	 * @param {int} $logIndex 日志索引
	 * @param {int} $callerModuleID 主调模块ID
	 * @param {int} $callerPort 主调模块端口
	 * @param {int} $callerIP 主调模块IP
	 * @return {int} 记录结果
	 */
	public static function end($callResult, $callReturn = 0, $callerFileLine = 0, $callerFileName = '', $callerFileLastModTime = 0, $logLevel = 0, $logIndex = -1, $callerModuleID = NULL, $callerPort = NULL, $callerIP = NULL)
	{
		$logLevel = max($logLevel, intval(self::getInstance()->get('logLevel', $logIndex)));

		$beginTime = floatval(self::getInstance()->get('beginTime', $logIndex));

		$endTime = $totalTime = 0;

		if (self::$logLevel >= $logLevel) {
			$endTime = microtime(true);

			$totalTime = intval(($endTime - $beginTime) * 1000);
		}

		$logInfo = array('endTime' => $endTime, 'totalTime' => $totalTime, 'callResult' => $callResult, 'callReturn' => $callReturn, 'callerFileLine' => $callerFileLine, 'callerFileName' => $callerFileName, 'callerFileLastModTime' => $callerFileLastModTime,);

		if (!empty($callerModuleID)) {
			$logInfo['callerModuleID'] = $callerModuleID;
		}

		if (!empty($callerPort)) {
			$logInfo['callerPort'] = $callerPort;
		}

		if (!empty($callerIP)) {
			$logInfo['callerIP'] = intval(tphp_ip2long($callerIP));
		}

		$logInfo = self::getInstance()->update($logInfo, $logIndex);

		if (self::$logLevel >= $logLevel) {
			$logResult = self::log($logInfo);

			if ($logResult != 0) {
				self::logError("ModuleCalling LogError $logResult");
			}

			return $logResult;
		}

		return 0;
	}

	protected $logList = array();

	protected $default = array();

	/**
	 *  构造函数
	 *
	 * @param {array} $default 默认记录集
	 */
	function __construct($default = null)
	{
		if (is_array($default)) {
			$this->default = $default;
		}
	}

	/**
	 *  析构函数
	 */
	function __destruct()
	{
		//echo('/*');
		//var_dump($this->logList);
		//echo('*/');
	}

	/**
	 *  插入记录
	 *
	 * @param {array} $logInfo 记录
	 * @return {int} 记录索引
	 */
	public function insert($logInfo)
	{
		return array_push($this->logList, array_merge($this->default, $logInfo));
	}

	/**
	 *  更新记录
	 *
	 * @param {array} $logInfo 记录
	 * @param {int} $logIndex 记录索引
	 * @return {array} 记录
	 */
	public function update($logInfo, $logIndex = -1)
	{
		$logIndex = ($logIndex == -1) ? count($this->logList) - 1 : $logIndex;

		if (array_key_exists($logIndex, $this->logList)) {
			$this->logList[$logIndex] = array_merge($this->logList[$logIndex], $logInfo);

			return $this->logList[$logIndex];
		}

		return false;
	}

	/**
	 *  获取记录的值
	 *
	 * @param {array} $logInfo 记录名称
	 * @param {int} $logIndex 记录索引
	 * @return {mixed} 记录值
	 */
	public function get($logName, $logIndex = -1)
	{
		$logIndex = ($logIndex == -1) ? count($this->logList) - 1 : $logIndex;

		if (array_key_exists($logIndex, $this->logList)) {
			if (array_key_exists($logName, $this->logList[$logIndex])) {
				return $this->logList[$logIndex][$logName];
			}
		}

		return false;
	}
}