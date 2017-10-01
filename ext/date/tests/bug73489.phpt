--TEST--
Bug #73489: wrong timestamp when call setTimeZone multi times with UTC offset
--FILE--
<?php
// example 1 - Timestamp is changing
$datetime = new DateTime('2016-11-09 20:00:00', new DateTimeZone('UTC'));
var_dump($datetime->getTimestamp());
$datetime->setTimeZone(new DateTimeZone('-03:00'));
$datetime->setTimeZone(new DateTimeZone('-03:00'));
var_dump($datetime->getTimestamp());

// example 2 - Timestamp keeps if you use getTimestamp() before second setTimeZone() calls
$datetime = new DateTime('2016-11-09 20:00:00', new DateTimeZone('UTC'));
var_dump($datetime->getTimestamp());
$datetime->setTimeZone(new DateTimeZone('-03:00'));
$datetime->getTimestamp();
$datetime->setTimeZone(new DateTimeZone('-03:00'));
var_dump($datetime->getTimestamp());
?>
--EXPECT--
int(1478721600)
int(1478721600)
int(1478721600)
int(1478721600)
