--TEST--
Test date_isodate_set() function : basic functionality
--FILE--
<?php
/* Prototype  : DateTime date_isodate_set  ( DateTime $object  , int $year  , int $week  [, int $day  ] )
 * Description: Set a date according to the ISO 8601 standard - using weeks and day offsets rather than specific dates.
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTime::setISODate
 */

echo "*** Testing date_isodate_set() : basic functionality ***\n";

//Set the default time zone
date_default_timezone_set("Europe/London");

// Create a deate object
$datetime = date_create("2009-01-30 17:57:32");

// Which month is week 40 ?
date_isodate_set($datetime, 2008, 40);
echo "Week 40 of 2009 is in \"" . date_format($datetime, "F") . "\"\n";

// What date is week  week 30 day 3 ?
date_isodate_set($datetime, 2009, 30, 3);
echo "Week 30 day 3 of 2009 is \"" . date_format($datetime, "D M j") . "\"\n";

// What date was is last year  ?
date_isodate_set($datetime, 2008, 30, 3);
echo "..same day last year was \"" . date_format($datetime, "D M j") . "\"\n";

?>
===DONE===
--EXPECTF--
*** Testing date_isodate_set() : basic functionality ***
Week 40 of 2009 is in "September"
Week 30 day 3 of 2009 is "Wed Jul 22"
..same day last year was "Wed Jul 23"
===DONE===
