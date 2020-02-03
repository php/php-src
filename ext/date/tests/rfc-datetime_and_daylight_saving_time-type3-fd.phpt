--TEST--
RFC: DateTime and Daylight Saving Time Transitions (zone type 3, fd)
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

date_default_timezone_set('America/New_York');
$date_format = 'Y-m-d H:i:s T e';
$interval_format = 'P%dDT%hH';

/*
 * Forward Transitions, diff().
 */

$end   = new DateTime('2010-03-14 03:00:00');
$start = new DateTime('2010-03-14 01:59:59');
echo 'fd1 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format('PT%hH%iM%sS') . "\n";

$end   = new DateTime('2010-03-14 04:30:00');
$start = new DateTime('2010-03-13 04:30:00');
echo 'fd2 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-03-14 03:30:00');
$start = new DateTime('2010-03-13 04:30:00');
echo 'fd3 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-03-14 01:30:00');
$start = new DateTime('2010-03-13 04:30:00');
echo 'fd4 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-03-14 01:30:00');
$start = new DateTime('2010-03-13 01:30:00');
echo 'fd5 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-03-14 03:30:00');
$start = new DateTime('2010-03-13 03:30:00');
echo 'fd6 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-03-14 03:30:00');
$start = new DateTime('2010-03-13 02:30:00');
echo 'fd7 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";
?>
--EXPECT--
fd1 2010-03-14 03:00:00 EDT America/New_York - 2010-03-14 01:59:59 EST America/New_York = PT0H0M1S
fd2 2010-03-14 04:30:00 EDT America/New_York - 2010-03-13 04:30:00 EST America/New_York = P1DT0H
fd3 2010-03-14 03:30:00 EDT America/New_York - 2010-03-13 04:30:00 EST America/New_York = P0DT22H
fd4 2010-03-14 01:30:00 EST America/New_York - 2010-03-13 04:30:00 EST America/New_York = P0DT21H
fd5 2010-03-14 01:30:00 EST America/New_York - 2010-03-13 01:30:00 EST America/New_York = P1DT0H
fd6 2010-03-14 03:30:00 EDT America/New_York - 2010-03-13 03:30:00 EST America/New_York = P1DT0H
fd7 2010-03-14 03:30:00 EDT America/New_York - 2010-03-13 02:30:00 EST America/New_York = P1DT1H
