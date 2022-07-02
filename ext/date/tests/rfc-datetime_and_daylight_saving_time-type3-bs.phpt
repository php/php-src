--TEST--
RFC: DateTime and Daylight Saving Time Transitions (zone type 3, bs)
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

date_default_timezone_set('America/New_York');
$tz = new DateTimeZone('America/New_York');
$date_format = 'Y-m-d H:i:s T e';
$interval_format = 'P%dDT%hH';

/*
 * Backward Transitions, sub().
 */

$end   = new DateTime('2010-11-07 01:00:00 EST');
$end->setTimeZone($tz);
$interval_spec = 'PT1S';
$interval = new DateInterval($interval_spec);
echo 'bs1 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 04:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'bs2 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 03:30:00');
$interval_spec = 'PT24H';
$interval = new DateInterval($interval_spec);
echo 'bs3 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 02:30:00');
$interval_spec = 'PT23H';
$interval = new DateInterval($interval_spec);
echo 'bs4 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EST');
$end->setTimeZone($tz);
$interval_spec = 'PT22H';
$interval = new DateInterval($interval_spec);
echo 'bs5 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EDT');
$end->setTimeZone($tz);
$interval_spec = 'PT21H';
$interval = new DateInterval($interval_spec);
echo 'bs6 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'bs7 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EST');
$end->setTimeZone($tz);
$interval_spec = 'P1DT1H';
$interval = new DateInterval($interval_spec);
echo 'bs8 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 03:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'bs9 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 02:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'bs10 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

?>
--EXPECT--
bs1 2010-11-07 01:00:00 EST America/New_York - PT1S = 2010-11-07 01:59:59 EDT America/New_York
bs2 2010-11-07 04:30:00 EST America/New_York - P1D = 2010-11-06 04:30:00 EDT America/New_York
bs3 2010-11-07 03:30:00 EST America/New_York - PT24H = 2010-11-06 04:30:00 EDT America/New_York
bs4 2010-11-07 02:30:00 EST America/New_York - PT23H = 2010-11-06 04:30:00 EDT America/New_York
bs5 2010-11-07 01:30:00 EST America/New_York - PT22H = 2010-11-06 04:30:00 EDT America/New_York
bs6 2010-11-07 01:30:00 EDT America/New_York - PT21H = 2010-11-06 04:30:00 EDT America/New_York
bs7 2010-11-07 01:30:00 EDT America/New_York - P1D = 2010-11-06 01:30:00 EDT America/New_York
bs8 2010-11-07 01:30:00 EST America/New_York - P1DT1H = 2010-11-06 00:30:00 EDT America/New_York
bs9 2010-11-07 03:30:00 EST America/New_York - P1D = 2010-11-06 03:30:00 EDT America/New_York
bs10 2010-11-07 02:30:00 EST America/New_York - P1D = 2010-11-06 02:30:00 EDT America/New_York
