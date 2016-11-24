--TEST--
Bug 73581 number_format allows for -0.00
--FILE--
<?php

$number = -1.15E-15;

var_dump($number);
var_dump(number_format($number, 2));

?>
--EXPECT--
float(-1.15E-15)
string(4) "0.00"
