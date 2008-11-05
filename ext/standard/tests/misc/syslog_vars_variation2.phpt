--TEST--
define_syslog_variables() variation 2
--FILE--
<?php
define_syslog_variables();

$standard_types = Array(
			'LOG_EMERG', 
			'LOG_ALERT', 
			'LOG_CRIT', 
			'LOG_ERR', 
			'LOG_WARNING', 
			'LOG_NOTICE', 
			'LOG_INFO', 
			'LOG_DEBUG', 
			'LOG_KERN', 
			'LOG_USER', 
			'LOG_MAIL', 
			'LOG_DAEMON', 
			'LOG_AUTH', 
			'LOG_LPR', 
			'LOG_PID', 
			'LOG_CONS', 
			'LOG_ODELAY', 
			'LOG_NDELAY'
			);

foreach($standard_types as $logtype)
{
	var_dump(isset(${$logtype}));
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
