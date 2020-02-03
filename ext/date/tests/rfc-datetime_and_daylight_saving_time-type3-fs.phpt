--TEST--
RFC: DateTime and Daylight Saving Time Transitions (zone type 3, fs)
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
 * Forward Transitions, sub().
 */

$end   = new DateTime('2010-03-14 03:00:00');
$interval_spec = 'PT1S';
$interval = new DateInterval($interval_spec);
echo 'fs1 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-14 04:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fs2 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-14 03:30:00');
$interval_spec = 'PT22H';
$interval = new DateInterval($interval_spec);
echo 'fs3 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-14 01:30:00');
$interval_spec = 'PT21H';
$interval = new DateInterval($interval_spec);
echo 'fs4 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-14 01:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fs5 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-15 03:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fs6 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-15 02:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fs7 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";
?>
--EXPECT--
fs1 2010-03-14 03:00:00 EDT America/New_York - PT1S = 2010-03-14 01:59:59 EST America/New_York
fs2 2010-03-14 04:30:00 EDT America/New_York - P1D = 2010-03-13 04:30:00 EST America/New_York
fs3 2010-03-14 03:30:00 EDT America/New_York - PT22H = 2010-03-13 04:30:00 EST America/New_York
fs4 2010-03-14 01:30:00 EST America/New_York - PT21H = 2010-03-13 04:30:00 EST America/New_York
fs5 2010-03-14 01:30:00 EST America/New_York - P1D = 2010-03-13 01:30:00 EST America/New_York
fs6 2010-03-15 03:30:00 EDT America/New_York - P1D = 2010-03-14 03:30:00 EDT America/New_York
fs7 2010-03-15 02:30:00 EDT America/New_York - P1D = 2010-03-14 03:30:00 EDT America/New_York
