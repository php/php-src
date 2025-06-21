--TEST--
Test abs() function : basic functionality
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = abs($value);
    var_dump($res);
}
?>
--EXPECT--
int(23)
int(23)
float(23.45)
float(23.45)
float(23.45)
int(10)
float(10.3)
float(3950.5)
