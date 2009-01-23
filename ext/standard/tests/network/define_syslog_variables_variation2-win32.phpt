--TEST--
Test define_syslog_variables() function : variation
--INI--
define_syslog_variables = false
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != "WIN")
  die("skip Only run on Windows");
?> 
--FILE--
<?php
/* Prototype  : void define_syslog_variables(void)
 * Description: Initializes all syslog-related variables 
 * Source code: ext/standard/syslog.c
 * Alias to functions: 
 */

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

// show variables not defined
foreach($log_variables as $log_var) {
   if (isset($$log_var)) {
      $failed = true;
      echo "FAILED: variable defined\n";
   }
} 

if ($failed == false) {
   echo "PASSED\n";
}
?>
===DONE===
--EXPECT--
*** Testing define_syslog_variables() : variation ***
PASSED
===DONE===
