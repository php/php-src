--TEST--
Bug #68161 - Relative offsets not cleared when setting a timestamp
--FILE--
<?php
ini_set('date.timezone', 'UTC');

$time_start = new DateTime('2014-10-02');

echo $time_start->format('d-m-Y') . "\n";

$time_start->modify('first day of this month');

echo $time_start->format('d') . "\n";

$time_start->setTimestamp(1412745640);

echo $time_start->format('d-m-Y') . "\n";
--EXPECTF--
02-10-2014
01
08-10-2014
