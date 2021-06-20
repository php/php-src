--TEST--
Bug #66257 (strtotime doesn't use current timezone on '4 thursdays')
--XFAIL--
'4 thursdays' changes time of day, but others don't
--INI--
date.timezone=America/Chicago
--FILE--
<?php
echo date_default_timezone_get() . "\n";
date_default_timezone_set('UTC');
$timestamp = mktime(0,0,0,11,1,2014);

$four_thursdays = strtotime('4 thursdays', $timestamp) . "\n";
$first_thursday = strtotime('first thursday', $timestamp) . "\n";
$plus_three_weeks = strtotime('+ 3 weeks', $timestamp) . "\n";
$first_thursday_plus_three_weeks = strtotime('first thursday + 3 weeks', $timestamp) . "\n";

echo 'base:                     ', date(DateTime::ISO8601 . ' e T', $timestamp), "\n";
echo '4 thursdays:              ', date(DateTime::ISO8601 . ' e T', $four_thursdays), "\n";
echo 'first thursday:           ', date(DateTime::ISO8601 . ' e T', $first_thursday), "\n";
echo '+ 3 weeks:                ', date(DateTime::ISO8601 . ' e T', $plus_three_weeks), "\n";
echo 'first thursday + 3 weeks: ', date(DateTime::ISO8601 . ' e T', $first_thursday_plus_three_weeks), "\n";
?>
--EXPECT--
