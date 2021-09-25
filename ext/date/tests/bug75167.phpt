--TEST--
Bug #75167 (DateTime::add does only care about backward DST transition, not forward)
--FILE--
<?php
$tz = new DateTimeZone('Europe/London'); // A timezone that has DST

$five_hours_interval = new DateInterval('PT5H');

$date = new DateTime("2014-3-30 00:00:00", $tz);

// Add five hours and subtract 5 hours. The $newDate should then equal the date.
$five_hours_later = (clone $date)->add($five_hours_interval);
$newDate = (clone $five_hours_later)->sub($five_hours_interval);

echo $date->format('c') . "\n";
echo $newDate->format('c');
?>
--EXPECT--
2014-03-30T00:00:00+00:00
2014-03-30T00:00:00+00:00
