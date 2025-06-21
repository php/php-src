--TEST--
Test variations in usage of sqrt()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = sqrt($value);
    var_dump($res);
}

?>
--EXPECT--
float(4.795831523312719)
float(NAN)
float(4.8425200051213)
float(4.8425200051213)
float(NAN)
float(3.1622776601683795)
float(3.2093613071762426)
float(62.853003110432205)
