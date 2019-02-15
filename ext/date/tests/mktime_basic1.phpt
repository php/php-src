--TEST--
Test mktime() function : basic functionality
--FILE--
<?php
/* Prototype  : int mktime  ([ int $hour= date("H")  [, int $minute= date("i")  [, int $second= date("s")  [, int $month= date("n")  [, int $day= date("j")  [, int $year= date("Y")  [, int $is_dst= -1  ]]]]]]] )
 * Description: Get Unix timestamp for a date
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

//Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing DateTime::modify() : basic functionality ***\n";

$hour = 10;
$minute = 30;
$sec = 45;
$month = 7;
$day = 2;
$year = 1963;

var_dump( mktime($hour) );
var_dump( mktime($hour, $minute) );
var_dump( mktime($hour, $minute, $sec) );
var_dump( mktime($hour, $minute, $sec, $month) );
var_dump( mktime($hour, $minute, $sec, $month, $day) );
var_dump( mktime($hour, $minute, $sec, $month, $day, $year) );

?>
===DONE===
--EXPECTF--
*** Testing DateTime::modify() : basic functionality ***
int(%i)
int(%i)
int(%i)
int(%i)
int(%i)
int(%i)
===DONE===
