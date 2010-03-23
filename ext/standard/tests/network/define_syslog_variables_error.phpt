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

Deprecated: Function define_syslog_variables() is deprecated in %s on line %d

Warning: define_syslog_variables() expects exactly 0 parameters, 1 given in %s on line %d
NULL
===DONE===
