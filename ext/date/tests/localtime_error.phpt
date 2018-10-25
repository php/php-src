--TEST--
Test localtime() function : error conditions
--FILE--
<?php
/* Prototype  : array localtime([int timestamp [, bool associative_array]])
 * Description: Returns the results of the C system call localtime as an associative array
 * if the associative_array argument is set to 1 other wise it is a regular array
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing localtime() : error conditions ***\n";

echo "\n-- Testing localtime() function with more than expected no. of arguments --\n";
$timestamp = gmmktime(8, 8, 8, 8, 8, 2008);
$assoc = true;
$extra_arg = 10;
var_dump( localtime($timestamp, $assoc, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing localtime() : error conditions ***

-- Testing localtime() function with more than expected no. of arguments --

Warning: localtime() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
===DONE===
