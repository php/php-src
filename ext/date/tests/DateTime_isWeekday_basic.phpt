--TEST--
Test DateTime::isWeekday() function : basic functionality
--FILE--
<?php
// Set the default time zone
date_default_timezone_set("UTC");

echo "*** Testing DateTime::isWeekday() : basic functionality ***\n";

// Saturday (weekend)
$date = new DateTime("2025-09-06");
echo "Saturday 2025-09-06 is weekday: ";
var_dump($date->isWeekday());

// Sunday (weekend)
$date = new DateTime("2025-09-07");
echo "Sunday 2025-09-07 is weekday: ";
var_dump($date->isWeekday());

// Monday (weekday)
$date = new DateTime("2025-09-08");
echo "Monday 2025-09-08 is weekday: ";
var_dump($date->isWeekday());

// Tuesday (weekday)
$date = new DateTime("2025-09-09");
echo "Tuesday 2025-09-09 is weekday: ";
var_dump($date->isWeekday());

// Test with DateTimeImmutable
echo "\n*** Testing DateTimeImmutable::isWeekday() : basic functionality ***\n";

// Saturday (weekend)
$date = new DateTimeImmutable("2025-09-06");
echo "Saturday 2025-09-06 is weekday: ";
var_dump($date->isWeekday());

// Sunday (weekend)
$date = new DateTimeImmutable("2025-09-07");
echo "Sunday 2025-09-07 is weekday: ";
var_dump($date->isWeekday());

// Monday (weekday)
$date = new DateTimeImmutable("2025-09-08");
echo "Monday 2025-09-08 is weekday: ";
var_dump($date->isWeekday());

// Tuesday (weekday)
$date = new DateTimeImmutable("2025-09-09");
echo "Tuesday 2025-09-09 is weekday: ";
var_dump($date->isWeekday());
?>
--EXPECT--
*** Testing DateTime::isWeekday() : basic functionality ***
Saturday 2025-09-06 is weekday: bool(false)
Sunday 2025-09-07 is weekday: bool(false)
Monday 2025-09-08 is weekday: bool(true)
Tuesday 2025-09-09 is weekday: bool(true)

*** Testing DateTimeImmutable::isWeekday() : basic functionality ***
Saturday 2025-09-06 is weekday: bool(false)
Sunday 2025-09-07 is weekday: bool(false)
Monday 2025-09-08 is weekday: bool(true)
Tuesday 2025-09-09 is weekday: bool(true)
