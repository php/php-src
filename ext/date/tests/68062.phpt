--TEST--
DateTimeZone::getOffset() accepts a DateTimeInterface object
--FILE--
<?php

$tz = new DateTimeZone('Europe/London');
$dt = new DateTimeImmutable('2014-09-20', $tz);

echo $tz->getOffset($dt);
echo $tz->getOffset(1);
--EXPECTF--
3600
Warning: DateTimeZone::getOffset() expects parameter 1 to be DateTimeInterface, integer given in %s
