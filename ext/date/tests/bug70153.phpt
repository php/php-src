--TEST--
Bug #70153 (\DateInterval incorrectly unserialized)
--FILE--
<?php
$i1 = \DateInterval::createFromDateString('+1 month');
$i2 = unserialize(serialize($i1));
var_dump($i1->days, $i2->days);
var_dump($i2->special_amount, $i2->special_amount);
?>
--EXPECT--
bool(false)
bool(false)
int(0)
int(0)
