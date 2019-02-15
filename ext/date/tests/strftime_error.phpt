--TEST--
Test strftime() function : error conditions
--FILE--
<?php
/* Prototype  : string strftime(string format [, int timestamp])
 * Description: Format a local time/date according to locale settings
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing strftime() : error conditions ***\n";

date_default_timezone_set("Asia/Calcutta");
//Test strftime with one more than the expected number of arguments
echo "\n-- Testing strftime() function with more than expected no. of arguments --\n";
$format = '%b %d %Y %H:%M:%S';
$timestamp = mktime(8, 8, 8, 8, 8, 2008);
$extra_arg = 10;
var_dump( strftime($format, $timestamp, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing strftime() : error conditions ***

-- Testing strftime() function with more than expected no. of arguments --

Warning: strftime() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
===DONE===
