--TEST--
Bug GH-10583 (DateTime modify with tz pattern should not update linked timezone)
--FILE--
<?php

$dt = new DateTime('2015-01-01 00:00:00+00:00');
var_dump($dt->format('c'));
var_dump($dt->modify('+1 s')->format('c'));

$dt = new DateTimeImmutable('2015-01-01 00:00:00+00:00');
var_dump($dt->format('c'));
var_dump($dt->modify('+1 s')->format('c'));
?>
--EXPECT--
string(25) "2015-01-01T00:00:00+00:00"
string(25) "2015-01-01T00:00:00+00:00"
string(25) "2015-01-01T00:00:00+00:00"
string(25) "2015-01-01T00:00:00+00:00"
