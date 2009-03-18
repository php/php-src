--TEST--
Test new DateTimeZone() : error conditions 
--FILE--
<?php
/* Prototype  : DateTimeZone::__construct  ( string $timezone  )
 * Description: Returns new DateTimeZone object
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */
//Set the default time zone 
date_default_timezone_set("GMT");

echo "*** Testing DateTimeZone() : error conditions ***\n";

echo "\n-- Testing new DateTimeZone() with more than expected no. of arguments --\n";
$timezone = "GMT";
$extra_arg = 99;
var_dump( new DateTimeZone($timezone, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing DateTimeZone() : error conditions ***

-- Testing new DateTimeZone() with more than expected no. of arguments --

Fatal error: Uncaught exception 'Exception' with message 'DateTimeZone::__construct() expects exactly 1 parameter, 2 given' in %s:%d
Stack trace:
#0 %s(%d): DateTimeZone->__construct('GMT', 99)
#1 {main}
  thrown in %s on line %d
  