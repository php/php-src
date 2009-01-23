--TEST--
Test define_syslog_variables() function : error conditions 
--FILE--
<?php
/* Prototype  : void define_syslog_variables(void)
 * Description: Initializes all syslog-related variables 
 * Source code: ext/standard/syslog.c
 * Alias to functions: 
 */

echo "*** Testing define_syslog_variables() : error conditions ***\n";

// One argument
echo "\n-- Testing define_syslog_variables() function with one argument --\n";
$extra_arg = 10;
var_dump( define_syslog_variables($extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing define_syslog_variables() : error conditions ***

-- Testing define_syslog_variables() function with one argument --

Warning: Wrong parameter count for define_syslog_variables() in %s on line %d
NULL
===DONE===
