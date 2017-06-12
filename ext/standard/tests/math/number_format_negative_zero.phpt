--TEST--
Prevent number_format from returning negative zero
--FILE--
<?php

$number = -1.15E-15;

var_dump($number);
var_dump(number_format($number, 2));
var_dump(number_format(-0.01, 2));

?>
--EXPECT--
float(-1.15E-15)
string(4) "0.00"
string(5) "-0.01"
