--TEST--
Bug #55253 (DateTime::add() and sub() result -1 hour on objects with time zone type 2)
--CREDITS--
Daniel Convissor <danielc@php.net>
--FILE--
<?php

date_default_timezone_set('America/New_York');

$interval = new DateInterval('PT2H1M');

$date3 = new DateTime('2010-10-04 02:18:48');
$date2 = new DateTime('2010-10-04 02:18:48 EDT');

echo 'Zone Type 3: ' . $date3->format('Y-m-d H:i:s T') . "\n";
echo 'Zone Type 2: ' . $date2->format('Y-m-d H:i:s T') . "\n";

echo $interval->format('Add %h hours %i minutes') . "\n";
$date3->add($interval);
$date2->add($interval);

echo 'Zone Type 3: ' . $date3->format('Y-m-d H:i:s T') . "\n";
echo 'Zone Type 2: ' . $date2->format('Y-m-d H:i:s T') . "\n";

// Try subtracting from expected result.
$date3 = new DateTime('2010-10-04 04:19:48');
$date2 = new DateTime('2010-10-04 04:19:48 EDT');

echo $interval->format('Subtract %h hours %i minutes from expected') . "\n";
$date3->sub($interval);
$date2->sub($interval);

echo 'Zone Type 3: ' . $date3->format('Y-m-d H:i:s T') . "\n";
echo 'Zone Type 2: ' . $date2->format('Y-m-d H:i:s T') . "\n";

?>
--EXPECT--
Zone Type 3: 2010-10-04 02:18:48 EDT
Zone Type 2: 2010-10-04 02:18:48 EDT
Add 2 hours 1 minutes
Zone Type 3: 2010-10-04 04:19:48 EDT
Zone Type 2: 2010-10-04 04:19:48 EDT
Subtract 2 hours 1 minutes from expected
Zone Type 3: 2010-10-04 02:18:48 EDT
Zone Type 2: 2010-10-04 02:18:48 EDT
