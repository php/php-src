--TEST--
RFC: DateTime and Daylight Saving Time Transitions (zone type 3, bd2)
--CREDITS--
Daniel Convissor <danielc@php.net>
--XFAIL--
Still not quite right
--FILE--
<?php

date_default_timezone_set('America/New_York');
$date_format = 'Y-m-d H:i:s T e';
$interval_format = 'P%dDT%hH';

/*
 * For backward transitions, must create objects with zone type 2
 * where specifying Daylight or Standard time is required
 * then converting them to zone type 3.
 */

$tz = new DateTimeZone('America/New_York');

/*
 * Backward Transitions, diff().
 */

$end   = new DateTime('2010-11-07 05:30:00');
$end->setTimeZone($tz);
$start = new DateTime('2010-11-06 04:30:59');
echo 'bd0 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
	. ' = ' . $start->diff($end)->format('P%dDT%hH%iM%sS') . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EST');
$end->setTimeZone($tz);
$start = new DateTime('2010-11-06 04:30:00');
echo 'bd5 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
	. ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EDT');
$end->setTimeZone($tz);
$start = new DateTime('2010-11-06 04:30:00');
echo 'bd6 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
	. ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EST');
$end->setTimeZone($tz);
$start = new DateTime('2010-11-06 01:30:00');
echo 'bd8 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
	. ' = ' . $start->diff($end)->format($interval_format) . "\n";

echo "\n";
?>
--EXPECT--
bd0 2010-11-07 01:00:00 EST America/New_York - 2010-11-07 01:59:59 EDT America/New_York = PT0H0M1S
bd5 2010-11-07 01:30:00 EST America/New_York - 2010-11-06 04:30:00 EDT America/New_York = P0DT22H
bd6 2010-11-07 01:30:00 EDT America/New_York - 2010-11-06 04:30:00 EDT America/New_York = P0DT21H
bd8 2010-11-07 01:30:00 EST America/New_York - 2010-11-06 01:30:00 EDT America/New_York = P1DT1H
