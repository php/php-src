--TEST--
Bug #70810: DateInterval equals every other DateInterval
--FILE--
<?php

$i1 = new DateInterval('P1D');
$i2 = new DateInterval('PT1H');
var_dump($i1 == $i2);
var_dump($i1 < $i2);
var_dump($i1 > $i2);

$i2 = new DateInterval('P1D');
var_dump($i1 == $i2);
var_dump($i1 < $i2);
var_dump($i1 > $i2);

?>
--EXPECTF--
Warning: Cannot compare DateInterval objects in %s on line %d
bool(false)

Warning: Cannot compare DateInterval objects in %s on line %d
bool(false)

Warning: Cannot compare DateInterval objects in %s on line %d
bool(false)

Warning: Cannot compare DateInterval objects in %s on line %d
bool(false)

Warning: Cannot compare DateInterval objects in %s on line %d
bool(false)

Warning: Cannot compare DateInterval objects in %s on line %d
bool(false)
