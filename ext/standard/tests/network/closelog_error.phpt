--TEST--
Test closelog() function : error conditions 
--FILE--
<?php
/* Prototype  : bool closelog(void)
 * Description: Close connection to system logger 
 * Source code: ext/standard/syslog.c
 * Alias to functions: 
 */

echo "*** Testing closelog() : error conditions ***\n";

// One argument
echo "\n-- Testing closelog() function with one argument --\n";
$extra_arg = 10;;
var_dump( closelog($extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing closelog() : error conditions ***

-- Testing closelog() function with one argument --

Warning: closelog() expects exactly 0 parameters, 1 given in %s on line %d
NULL
===DONE===
