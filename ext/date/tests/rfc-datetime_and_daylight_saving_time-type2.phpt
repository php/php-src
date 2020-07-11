--TEST--
RFC: DateTime and Daylight Saving Time Transitions (zone type 2)
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

$end   = new DateTime('2010-03-14 03:00:00 EDT');
$start = new DateTime('2010-03-14 01:59:59 EST');
echo 'fd1 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format('PT%hH%iM%sS') . "\n";

$end   = new DateTime('2010-03-14 04:30:00 EDT');
$start = new DateTime('2010-03-13 04:30:00 EST');
echo 'fd2 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-03-14 03:30:00 EDT');
$start = new DateTime('2010-03-13 04:30:00 EST');
echo 'fd3 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-03-14 01:30:00 EST');
$start = new DateTime('2010-03-13 04:30:00 EST');
echo 'fd4 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-03-14 01:30:00 EST');
$start = new DateTime('2010-03-13 01:30:00 EST');
echo 'fd5 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-03-14 03:30:00 EDT');
$start = new DateTime('2010-03-13 03:30:00 EST');
echo 'fd6 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-03-14 03:30:00 EDT');
$start = new DateTime('2010-03-13 02:30:00 EST');
echo 'fd7 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

echo "\n";

/*
 * Forward Transitions, add().
 */

$start = new DateTime('2010-03-14 01:59:59 EST');
$interval_spec = 'PT1S';
$interval = new DateInterval($interval_spec);
echo 'fa1 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-03-13 04:30:00 EST');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fa2 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-03-13 04:30:00 EST');
$interval_spec = 'PT22H';
$interval = new DateInterval($interval_spec);
echo 'fa3 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-03-13 04:30:00 EST');
$interval_spec = 'PT21H';
$interval = new DateInterval($interval_spec);
echo 'fa4 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-03-13 01:30:00 EST');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fa5 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-03-13 02:30:00 EST');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fa6 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

echo "\n";

/*
 * Forward Transitions, sub().
 */

$end   = new DateTime('2010-03-14 03:00:00 EDT');
$interval_spec = 'PT1S';
$interval = new DateInterval($interval_spec);
echo 'fs1 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-14 04:30:00 EDT');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fs2 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-14 03:30:00 EDT');
$interval_spec = 'PT22H';
$interval = new DateInterval($interval_spec);
echo 'fs3 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-14 01:30:00 EST');
$interval_spec = 'PT21H';
$interval = new DateInterval($interval_spec);
echo 'fs4 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-14 01:30:00 EST');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fs5 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-15 03:30:00 EDT');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fs6 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-03-15 02:30:00 EDT');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'fs7 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

echo "\n";

/*
 * Backward Transitions, diff().
 */

$end   = new DateTime('2010-11-07 01:00:00 EST');
$start = new DateTime('2010-11-07 01:59:59 EDT');
echo 'bd1 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format('PT%hH%iM%sS') . "\n";

$end   = new DateTime('2010-11-07 04:30:00 EST');
$start = new DateTime('2010-11-06 04:30:00 EDT');
echo 'bd2 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-11-07 03:30:00 EST');
$start = new DateTime('2010-11-06 04:30:00 EDT');
echo 'bd3 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-11-07 02:30:00 EST');
$start = new DateTime('2010-11-06 04:30:00 EDT');
echo 'bd4 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EST');
$start = new DateTime('2010-11-06 04:30:00 EDT');
echo 'bd5 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EDT');
$start = new DateTime('2010-11-06 04:30:00 EDT');
echo 'bd6 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EDT');
$start = new DateTime('2010-11-06 01:30:00 EDT');
echo 'bd7 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EST');
$start = new DateTime('2010-11-06 01:30:00 EDT');
echo 'bd8 ' . $end->format($date_format) . ' - ' . $start->format($date_format)
    . ' = ' . $start->diff($end)->format($interval_format) . "\n";

echo "\n";

/*
 * Backward Transitions, add().
 */

$start = new DateTime('2010-11-07 01:59:59 EDT');
$interval_spec = 'PT1S';
$interval = new DateInterval($interval_spec);
echo 'ba1 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00 EDT');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'ba2 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00 EDT');
$interval_spec = 'PT24H';
$interval = new DateInterval($interval_spec);
echo 'ba3 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00 EDT');
$interval_spec = 'PT23H';
$interval = new DateInterval($interval_spec);
echo 'ba4 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00 EDT');
$interval_spec = 'PT22H';
$interval = new DateInterval($interval_spec);
echo 'ba5 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00 EDT');
$interval_spec = 'PT21H';
$interval = new DateInterval($interval_spec);
echo 'ba6 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 01:30:00 EDT');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'ba7 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 01:30:00 EDT');
$interval_spec = 'P1DT1H';
$interval = new DateInterval($interval_spec);
echo 'ba8 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 04:30:00 EDT');
$interval_spec = 'PT25H';
$interval = new DateInterval($interval_spec);
echo 'ba9 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 03:30:00 EDT');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'ba10 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

$start = new DateTime('2010-11-06 02:30:00 EDT');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'ba11 ' . $start->format($date_format) . " + $interval_spec = "
    . $start->add($interval)->format($date_format) . "\n";

echo "\n";

/*
 * Backward Transitions, sub().
 */

$end   = new DateTime('2010-11-07 01:00:00 EST');
$interval_spec = 'PT1S';
$interval = new DateInterval($interval_spec);
echo 'bs1 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 04:30:00 EST');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'bs2 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 03:30:00 EST');
$interval_spec = 'PT24H';
$interval = new DateInterval($interval_spec);
echo 'bs3 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 02:30:00 EST');
$interval_spec = 'PT23H';
$interval = new DateInterval($interval_spec);
echo 'bs4 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EST');
$interval_spec = 'PT22H';
$interval = new DateInterval($interval_spec);
echo 'bs5 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EDT');
$interval_spec = 'PT21H';
$interval = new DateInterval($interval_spec);
echo 'bs6 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EDT');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'bs7 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 01:30:00 EST');
$interval_spec = 'P1DT1H';
$interval = new DateInterval($interval_spec);
echo 'bs8 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 03:30:00 EST');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'bs9 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

$end   = new DateTime('2010-11-07 02:30:00 EST');
$interval_spec = 'P1D';
$interval = new DateInterval($interval_spec);
echo 'bs10 ' . $end->format($date_format) . " - $interval_spec = "
    . $end->sub($interval)->format($date_format) . "\n";

?>
--EXPECT--
fd1 2010-03-14 03:00:00 EDT EDT - 2010-03-14 01:59:59 EST EST = PT0H0M1S
fd2 2010-03-14 04:30:00 EDT EDT - 2010-03-13 04:30:00 EST EST = P0DT23H
fd3 2010-03-14 03:30:00 EDT EDT - 2010-03-13 04:30:00 EST EST = P0DT22H
fd4 2010-03-14 01:30:00 EST EST - 2010-03-13 04:30:00 EST EST = P0DT21H
fd5 2010-03-14 01:30:00 EST EST - 2010-03-13 01:30:00 EST EST = P1DT0H
fd6 2010-03-14 03:30:00 EDT EDT - 2010-03-13 03:30:00 EST EST = P0DT23H
fd7 2010-03-14 03:30:00 EDT EDT - 2010-03-13 02:30:00 EST EST = P1DT0H

fa1 2010-03-14 01:59:59 EST EST + PT1S = 2010-03-14 02:00:00 EST EST
fa2 2010-03-13 04:30:00 EST EST + P1D = 2010-03-14 04:30:00 EST EST
fa3 2010-03-13 04:30:00 EST EST + PT22H = 2010-03-14 02:30:00 EST EST
fa4 2010-03-13 04:30:00 EST EST + PT21H = 2010-03-14 01:30:00 EST EST
fa5 2010-03-13 01:30:00 EST EST + P1D = 2010-03-14 01:30:00 EST EST
fa6 2010-03-13 02:30:00 EST EST + P1D = 2010-03-14 02:30:00 EST EST

fs1 2010-03-14 03:00:00 EDT EDT - PT1S = 2010-03-14 02:59:59 EDT EDT
fs2 2010-03-14 04:30:00 EDT EDT - P1D = 2010-03-13 04:30:00 EDT EDT
fs3 2010-03-14 03:30:00 EDT EDT - PT22H = 2010-03-13 05:30:00 EDT EDT
fs4 2010-03-14 01:30:00 EST EST - PT21H = 2010-03-13 04:30:00 EST EST
fs5 2010-03-14 01:30:00 EST EST - P1D = 2010-03-13 01:30:00 EST EST
fs6 2010-03-15 03:30:00 EDT EDT - P1D = 2010-03-14 03:30:00 EDT EDT
fs7 2010-03-15 02:30:00 EDT EDT - P1D = 2010-03-14 02:30:00 EDT EDT

bd1 2010-11-07 01:00:00 EST EST - 2010-11-07 01:59:59 EDT EDT = PT0H0M1S
bd2 2010-11-07 04:30:00 EST EST - 2010-11-06 04:30:00 EDT EDT = P1DT1H
bd3 2010-11-07 03:30:00 EST EST - 2010-11-06 04:30:00 EDT EDT = P1DT0H
bd4 2010-11-07 02:30:00 EST EST - 2010-11-06 04:30:00 EDT EDT = P0DT23H
bd5 2010-11-07 01:30:00 EST EST - 2010-11-06 04:30:00 EDT EDT = P0DT22H
bd6 2010-11-07 01:30:00 EDT EDT - 2010-11-06 04:30:00 EDT EDT = P0DT21H
bd7 2010-11-07 01:30:00 EDT EDT - 2010-11-06 01:30:00 EDT EDT = P1DT0H
bd8 2010-11-07 01:30:00 EST EST - 2010-11-06 01:30:00 EDT EDT = P1DT1H

ba1 2010-11-07 01:59:59 EDT EDT + PT1S = 2010-11-07 02:00:00 EDT EDT
ba2 2010-11-06 04:30:00 EDT EDT + P1D = 2010-11-07 04:30:00 EDT EDT
ba3 2010-11-06 04:30:00 EDT EDT + PT24H = 2010-11-07 04:30:00 EDT EDT
ba4 2010-11-06 04:30:00 EDT EDT + PT23H = 2010-11-07 03:30:00 EDT EDT
ba5 2010-11-06 04:30:00 EDT EDT + PT22H = 2010-11-07 02:30:00 EDT EDT
ba6 2010-11-06 04:30:00 EDT EDT + PT21H = 2010-11-07 01:30:00 EDT EDT
ba7 2010-11-06 01:30:00 EDT EDT + P1D = 2010-11-07 01:30:00 EDT EDT
ba8 2010-11-06 01:30:00 EDT EDT + P1DT1H = 2010-11-07 02:30:00 EDT EDT
ba9 2010-11-06 04:30:00 EDT EDT + PT25H = 2010-11-07 05:30:00 EDT EDT
ba10 2010-11-06 03:30:00 EDT EDT + P1D = 2010-11-07 03:30:00 EDT EDT
ba11 2010-11-06 02:30:00 EDT EDT + P1D = 2010-11-07 02:30:00 EDT EDT

bs1 2010-11-07 01:00:00 EST EST - PT1S = 2010-11-07 00:59:59 EST EST
bs2 2010-11-07 04:30:00 EST EST - P1D = 2010-11-06 04:30:00 EST EST
bs3 2010-11-07 03:30:00 EST EST - PT24H = 2010-11-06 03:30:00 EST EST
bs4 2010-11-07 02:30:00 EST EST - PT23H = 2010-11-06 03:30:00 EST EST
bs5 2010-11-07 01:30:00 EST EST - PT22H = 2010-11-06 03:30:00 EST EST
bs6 2010-11-07 01:30:00 EDT EDT - PT21H = 2010-11-06 04:30:00 EDT EDT
bs7 2010-11-07 01:30:00 EDT EDT - P1D = 2010-11-06 01:30:00 EDT EDT
bs8 2010-11-07 01:30:00 EST EST - P1DT1H = 2010-11-06 00:30:00 EST EST
bs9 2010-11-07 03:30:00 EST EST - P1D = 2010-11-06 03:30:00 EST EST
bs10 2010-11-07 02:30:00 EST EST - P1D = 2010-11-06 02:30:00 EST EST
