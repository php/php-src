--TEST--
DateTimeZone::getOffset() accepts a DateTimeInterface object
--FILE--
<?php

$tz = new DateTimeZone('Europe/London');
$dt = new DateTimeImmutable('2014-09-20', $tz);

echo $tz->getOffset($dt), "\n";
try {
    echo $tz->getOffset(1);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
--EXPECT--
3600
DateTimeZone::getOffset() expects argument #1 ($datetime) to be of type DateTimeInterface, int given
