--TEST--
Test DateTime::setDate() function : basic functionality
--FILE--
<?php
/* Prototype  : public DateTime DateTime::setDate  ( int $year  , int $month  , int $day  )
 * Description: Resets the current date of the DateTime object to a different date.
 * Source code: ext/date/php_date.c
 * Alias to functions: date_date_set()
 */

 //Set the default time zone
date_default_timezone_set("Europe/London");

echo "*** Testing DateTime::setDate() : basic functionality ***\n";

$datetime = new DateTime("2009-01-30 19:34:10");

echo $datetime->format(DATE_RFC2822) . "\n";

$datetime->setDate(2008, 02, 01);

echo $datetime->format(DATE_RFC2822) . "\n";

?>
===DONE===
--EXPECTF--
*** Testing DateTime::setDate() : basic functionality ***
Fri, 30 Jan 2009 19:34:10 +0000
Fri, 01 Feb 2008 19:34:10 +0000
===DONE===
