--TEST--
Bug #75222 DateInterval microseconds property always 0
--FILE--
<?php

$dt1 = new \DateTimeImmutable('2017-01-01T00:00:00.000000Z');
$dt2 = new \DateTimeImmutable('2017-01-01T00:00:00.123456Z');
$diff = $dt1->diff($dt2);
//var_dump($diff);
var_dump($diff->f);
var_dump(get_object_vars($diff)['f']);
var_dump($diff->f === get_object_vars($diff)['f']);
?>
--EXPECT--
float(0.123456)
float(0.123456)
bool(true)
