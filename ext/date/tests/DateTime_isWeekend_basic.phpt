--TEST--
Test DateTime::isWeekend() function : basic functionality
--FILE--
<?php
// Set the default time zone
date_default_timezone_set("UTC");

echo "*** Testing DateTime::isWeekend() : basic functionality ***\n";

// Saturday (weekend)
$date = new DateTime("2025-09-06");
echo "Saturday 2025-09-06 is weekend: ";
var_dump($date->isWeekend());

// Sunday (weekend)
$date = new DateTime("2025-09-07");
echo "Sunday 2025-09-07 is weekend: ";
var_dump($date->isWeekend());

// Monday (weekday)
$date = new DateTime("2025-09-08");
echo "Monday 2025-09-08 is weekend: ";
var_dump($date->isWeekend());

// Test with DateTimeImmutable
echo "\n*** Testing DateTimeImmutable::isWeekend() : basic functionality ***\n";

// Saturday (weekend)
$date = new DateTimeImmutable("2025-09-06");
echo "Saturday 2025-09-06 is weekend: ";
var_dump($date->isWeekend());

// Sunday (weekend)
$date = new DateTimeImmutable("2025-09-07");
echo "Sunday 2025-09-07 is weekend: ";
var_dump($date->isWeekend());

// Monday (weekday)
$date = new DateTimeImmutable("2025-09-08");
echo "Monday 2025-09-08 is weekend: ";
var_dump($date->isWeekend());
?>
--EXPECT--
*** Testing DateTime::isWeekend() : basic functionality ***
Saturday 2025-09-06 is weekend: bool(true)
Sunday 2025-09-07 is weekend: bool(true)
Monday 2025-09-08 is weekend: bool(false)

*** Testing DateTimeImmutable::isWeekend() : basic functionality ***
Saturday 2025-09-06 is weekend: bool(true)
Sunday 2025-09-07 is weekend: bool(true)
Monday 2025-09-08 is weekend: bool(false)
