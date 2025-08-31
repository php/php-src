--TEST--
Bug #81500 (Interval serialization regression since 7.3.14 / 7.4.2)
--FILE--
<?php
$interval = new DateInterval('PT1S');
$interval->f = -0.000001;
var_dump($interval->s, $interval->f);

$interval = unserialize(serialize($interval));
var_dump($interval->s, $interval->f);
?>
--EXPECT--
int(1)
float(-1.0E-6)
int(1)
float(-1.0E-6)
