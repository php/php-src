--TEST--
Bug #74298 (IntlDateFormatter->format() doesn't return microseconds/fractions)
--SKIPIF--
<?php if (!extension_loaded('intl')) print 'skip'; ?>
--FILE--
<?php
var_dump((new \DateTime('2017-01-01 01:02:03.123456'))->format('Y-m-d\TH:i:s.u'));

var_dump((new \IntlDateFormatter(
    'en-US',
    \IntlDateFormatter::FULL,
    \IntlDateFormatter::FULL,
    'UTC',
    \IntlDateFormatter::GREGORIAN,
    'yyyy-MM-dd HH:mm:ss.SSSSSS'
))->format(new \DateTime('2017-01-01 01:02:03.123456', new \DateTimeZone('UTC'))));

var_dump(datefmt_create(
    'en-US',
    \IntlDateFormatter::FULL,
    \IntlDateFormatter::FULL,
    'UTC',
    \IntlDateFormatter::GREGORIAN,
    'yyyy-MM-dd HH:mm:ss.SSSSSS'
)->format(new \DateTime('2017-01-01 01:02:03.123456', new \DateTimeZone('UTC'))));
?>
--EXPECTF--
string(26) "2017-01-01T01:02:03.123456"
string(26) "2017-01-01 01:02:03.123000"
string(26) "2017-01-01 01:02:03.123000"
