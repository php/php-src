--TEST--
Test mktime() function : error conditions 
--FILE--
<?php
/* Prototype  : int mktime  ([ int $hour= date("H")  [, int $minute= date("i")  [, int $second= date("s")  [, int $month= date("n")  [, int $day= date("j")  [, int $year= date("Y")  [, int $is_dst= -1  ]]]]]]] )
 * Description: Get Unix timestamp for a date
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */
error_reporting(E_ALL | E_STRICT);

//Set the default time zone 
date_default_timezone_set("Europe/London");

echo "*** Testing mktime() : error conditions ***\n";

echo "\n-- Testing mktime() function with Zero arguments --\n";
var_dump( mktime() );

echo "\n-- Testing mktime() function with more than expected no. of arguments --\n";
$hour = 10;
$minute = 30;
$sec = 45;
$month = 7;
$day = 2;
$year = 1963;
$is_dst = 0; 
$extra_arg = 10;
var_dump( mktime($hour, $minute, $sec, $month, $day, $year, $is_dst, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing mktime() : error conditions ***

-- Testing mktime() function with Zero arguments --

Strict Standards: mktime(): You should be using the time() function instead in %s on line %d
int(%d)

-- Testing mktime() function with more than expected no. of arguments --

Warning: mktime() expects at most 7 parameters, 8 given in %s on line %d
bool(false)
===DONE===
