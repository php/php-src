--TEST--
Test define_syslog_variables() function : variation
--FILE--
<?php
/* Prototype  : void define_syslog_variables(void)
 * Description: Initializes all syslog-related variables 
 * Source code: ext/standard/syslog.c
 * Alias to functions: 
 */
 
define_syslog_variables();

echo "*** Testing define_syslog_variables() : variation ***\n";

$log_constants = array(
   		LOG_EMERG, 
   		LOG_ALERT, 
   		LOG_CRIT, 
   		LOG_ERR, 
   		LOG_WARNING, 
   		LOG_NOTICE, 
   		LOG_INFO, 
   		LOG_DEBUG,
		LOG_KERN, 
		LOG_USER, 
		LOG_MAIL, 
		LOG_DAEMON, 
		LOG_AUTH, 
		LOG_SYSLOG, 
		LOG_LPR, 
		LOG_NEWS, 
		LOG_UUCP, 
		LOG_CRON, 
		LOG_AUTHPRIV,
		LOG_PID, 
		LOG_CONS, 
		LOG_ODELAY, 
		LOG_NDELAY, 
		LOG_NOWAIT, 
		LOG_PERROR,
);


$log_variables = array(
   		"LOG_EMERG", 
   		"LOG_ALERT", 
   		"LOG_CRIT", 
   		"LOG_ERR", 
   		"LOG_WARNING", 
   		"LOG_NOTICE", 
   		"LOG_INFO", 
   		"LOG_DEBUG",
		"LOG_KERN", 
		"LOG_USER", 
		"LOG_MAIL", 
		"LOG_DAEMON", 
		"LOG_AUTH", 
		"LOG_SYSLOG", 
		"LOG_LPR", 
		"LOG_NEWS", 
		"LOG_UUCP", 
		"LOG_CRON", 
		"LOG_AUTHPRIV",
		"LOG_PID", 
		"LOG_CONS", 
		"LOG_ODELAY", 
		"LOG_NDELAY", 
		"LOG_NOWAIT", 
		"LOG_PERROR",
);

error_reporting(E_ALL);
$failed = false;

// show variables defined
for ($t = 0; $t < count($log_variables); $t++) {
   if (isset($$log_variables[$t]) === false || $$log_variables[$t] != $log_constants[$t]) {
      $failed = true;
      echo "FAILED: $log_variables[$t] doesn't contain the correct value\n";
   }
}

if ($failed == false) {
   echo "PASSED\n";
}
?>
===DONE===
--EXPECTF--
Deprecated: Function define_syslog_variables() is deprecated in %s on line %d
*** Testing define_syslog_variables() : variation ***
PASSED
===DONE===
