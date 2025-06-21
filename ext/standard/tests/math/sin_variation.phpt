--TEST--
Test variations in usage of sin()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = sin($value);
    var_dump($res);
}

?>
--EXPECT--
float(-0.8462204041751706)
float(0.8462204041751706)
float(-0.9937407101726596)
float(-0.9937407101726596)
float(0.9937407101726596)
float(-0.5440211108893698)
float(-0.7676858097635825)
float(-0.9986084143974419)
