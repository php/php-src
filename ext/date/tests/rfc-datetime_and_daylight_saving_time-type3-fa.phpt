--TEST--
RFC: DateTime and Daylight Saving Time Transitions (zone type 3, fa)
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

date_default_timezone_set('America/New_York');
$date_format = 'Y-m-d H:i:s T e';
$interval_format = 'P%dDT%hH';

/*
 * Forward Transitions, add().
 */

$start = new DateTime('2010-03-14 01:59:59');
$interval_spec = 'PT1S';
$interval = new DateInterval($interval_spec);
echo 'fa1 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-03-13 04:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fa2 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-03-13 04:30:00');
$interval_spec = 'PT23H';
$interval = new DateInterval($interval_spec);
echo 'fa2.5 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-03-13 04:30:00');
$interval_spec = 'PT22H';
$interval = new DateInterval($interval_spec);
echo 'fa3 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-03-13 04:30:00');
$interval_spec = 'PT21H';
$interval = new DateInterval($interval_spec);
echo 'fa4 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-03-13 01:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fa5 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-03-13 02:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fa6 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";
?>
--EXPECT--
fa1 2010-03-14 01:59:59 EST America/New_York + PT1S = 2010-03-14 03:00:00 EDT America/New_York
fa2 2010-03-13 04:30:00 EST America/New_York + P1D = 2010-03-14 04:30:00 EDT America/New_York
fa2.5 2010-03-13 04:30:00 EST America/New_York + PT23H = 2010-03-14 04:30:00 EDT America/New_York
fa3 2010-03-13 04:30:00 EST America/New_York + PT22H = 2010-03-14 03:30:00 EDT America/New_York
fa4 2010-03-13 04:30:00 EST America/New_York + PT21H = 2010-03-14 01:30:00 EST America/New_York
fa5 2010-03-13 01:30:00 EST America/New_York + P1D = 2010-03-14 01:30:00 EST America/New_York
fa6 2010-03-13 02:30:00 EST America/New_York + P1D = 2010-03-14 03:30:00 EDT America/New_York
