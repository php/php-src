--TEST--
Test define_syslog_variables() function : basic functionality 
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip don't run on Windows");
?> 
--FILE--
<?php
/* Prototype  : void define_syslog_variables(void)
 * Description: Initializes all syslog-related variables 
 * Source code: ext/standard/syslog.c
 * Alias to functions: 
 */
 
echo "*** Testing define_syslog_variables() : basic functionality ***\n";

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
		
		LOG_LOCAL0, 
		LOG_LOCAL1, 
		LOG_LOCAL2, 
		LOG_LOCAL3, 
		LOG_LOCAL4, 
		LOG_LOCAL5, 
		LOG_LOCAL6, 
		LOG_LOCAL7		

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
		
		"LOG_LOCAL0", 
		"LOG_LOCAL1", 
		"LOG_LOCAL2", 
		"LOG_LOCAL3", 
		"LOG_LOCAL4", 
		"LOG_LOCAL5", 
		"LOG_LOCAL6", 
		"LOG_LOCAL7"		
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

var_dump( define_syslog_variables() );

// show variables now defined
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
*** Testing define_syslog_variables() : basic functionality ***

Deprecated: Function define_syslog_variables() is deprecated in %s on line %d
NULL
PASSED
===DONE===
