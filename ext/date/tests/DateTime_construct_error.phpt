--TEST--
Test new DateTime() : error conditions 
--FILE--
<?php
/* Prototype  : DateTime::__construct  ([ string $time="now"  [, DateTimeZone $timezone=NULL  ]] )
 * Description: Returns new DateTime object
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */
//Set the default time zone 
date_default_timezone_set("GMT");

echo "*** Testing date_create() : error conditions ***\n";

echo "\n-- Testing new DateTime() with more than expected no. of arguments --\n";
$time = "GMT";
$timezone  = timezone_open("GMT");
$extra_arg = 99;
var_dump( new DateTime($time, $timezone, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing date_create() : error conditions ***

-- Testing new DateTime() with more than expected no. of arguments --

Fatal error: Uncaught exception 'Exception' with message 'DateTime::__construct() expects at most 2 parameters, 3 given' in %s:%d
Stack trace:
#0 %s(%d): DateTime->__construct('GMT', Object(DateTimeZone), 99)
#1 {main}
  thrown in %s on line %d