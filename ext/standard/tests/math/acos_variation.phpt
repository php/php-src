--TEST--
Test variations in usage of acos()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = acos($value);
    var_dump($res);
}

?>
--EXPECT--
float(NAN)
float(NAN)
float(NAN)
float(NAN)
float(NAN)
float(NAN)
float(NAN)
float(NAN)
