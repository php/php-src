--TEST--
RFC: DateTime and Daylight Saving Time Transitions (zone type 3, ba)
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

date_default_timezone_set('America/New_York');
$date_format = 'Y-m-d H:i:s T e';
$interval_format = 'P%dDT%hH';

/*
 * Backward Transitions, add().
 */

$start = new DateTime('2010-11-07 01:59:59');
$interval_spec = 'PT1S';
$interval = new DateInterval($interval_spec);
echo 'ba1 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'ba2 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00');
$interval_spec = 'PT24H';
$interval = new DateInterval($interval_spec);
echo 'ba3 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00');
$interval_spec = 'PT23H';
$interval = new DateInterval($interval_spec);
echo 'ba4 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00');
$interval_spec = 'PT22H';
$interval = new DateInterval($interval_spec);
echo 'ba5 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00');
$interval_spec = 'PT21H';
$interval = new DateInterval($interval_spec);
echo 'ba6 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 01:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'ba7 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 01:30:00');
$interval_spec = 'P1DT1H';
$interval = new DateInterval($interval_spec);
echo 'ba8 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00');
$interval_spec = 'PT25H';
$interval = new DateInterval($interval_spec);
echo 'ba9 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 03:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'ba10 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 02:30:00');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'ba11 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

echo "\n";

?>
--EXPECT--
ba1 2010-11-07 01:59:59 EDT America/New_York + PT1S = 2010-11-07 01:00:00 EST America/New_York
ba2 2010-11-06 04:30:00 EDT America/New_York + P1D = 2010-11-07 04:30:00 EST America/New_York
ba3 2010-11-06 04:30:00 EDT America/New_York + PT24H = 2010-11-07 03:30:00 EST America/New_York
ba4 2010-11-06 04:30:00 EDT America/New_York + PT23H = 2010-11-07 02:30:00 EST America/New_York
ba5 2010-11-06 04:30:00 EDT America/New_York + PT22H = 2010-11-07 01:30:00 EST America/New_York
ba6 2010-11-06 04:30:00 EDT America/New_York + PT21H = 2010-11-07 01:30:00 EDT America/New_York
ba7 2010-11-06 01:30:00 EDT America/New_York + P1D = 2010-11-07 01:30:00 EDT America/New_York
ba8 2010-11-06 01:30:00 EDT America/New_York + P1DT1H = 2010-11-07 02:30:00 EST America/New_York
ba9 2010-11-06 04:30:00 EDT America/New_York + PT25H = 2010-11-07 04:30:00 EST America/New_York
ba10 2010-11-06 03:30:00 EDT America/New_York + P1D = 2010-11-07 03:30:00 EST America/New_York
ba11 2010-11-06 02:30:00 EDT America/New_York + P1D = 2010-11-07 02:30:00 EST America/New_York
