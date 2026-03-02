--TEST--
Bug #81273: Date interval calculation not correct
--FILE--
<?php

$time = '2000-01-01 00:00:00.000000';
$tz_aus = new DateTimeZone('Australia/Sydney');
$tz_us = new DateTimeZone('America/Los_Angeles');
$auz = new DateTime($time, $tz_aus);
$us = new DateTime($time, $tz_us);

$diff = $auz->diff($us);
// Should output int(19)
var_dump($diff->h);
?>
--EXPECT--
int(19)
