--TEST--
Test for bug #45543: DateTime::setTimezone can not set timezones without ID
--CREDITS--
Boro Sitnikovski <buritomath@yahoo.com>
--INI--
date.timezone = UTC
--FILE--
<?php
$d1 = new DateTime('2008-01-01 12:00:00 +02:00');
$d2 = new DateTime('2008-01-01 12:00:00 UTC');
echo $d1->format(DATE_ISO8601), PHP_EOL;
echo $d2->format(DATE_ISO8601), PHP_EOL;
$d2->setTimeZone($d1->getTimeZone());
echo $d1->format(DATE_ISO8601), PHP_EOL;
echo $d2->format(DATE_ISO8601), PHP_EOL;
?>
--EXPECT--
2008-01-01T12:00:00+0200
2008-01-01T12:00:00+0000
2008-01-01T12:00:00+0200
2008-01-01T12:00:00+0200
