--TEST--
Test variations in usage of deg2rad()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = deg2rad($value);
    var_dump($res);
}

?>
--EXPECT--
float(0.40142572795869574)
float(-0.40142572795869574)
float(0.40927970959267024)
float(0.40927970959267024)
float(-0.40927970959267024)
float(0.17453292519943295)
float(0.17976891295541594)
float(68.94923210003599)
