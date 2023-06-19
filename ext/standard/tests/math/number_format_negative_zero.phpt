--TEST--
Prevent number_format from returning negative zero
--FILE--
<?php

$number = -1.15E-15;
var_dump($number);
var_dump(number_format($number, 2));

$number = -0.01;
var_dump($number);
var_dump(number_format($number, 2));

$number = -1234567.89;
var_dump($number);
var_dump(number_format($number, -3));

$number = -1234321.09;
var_dump($number);
var_dump(number_format($number, -3));

?>
--EXPECT--
float(-1.15E-15)
string(4) "0.00"
float(-0.01)
string(5) "-0.01"
float(-1234567.89)
string(10) "-1,235,000"
float(-1234321.09)
string(10) "-1,234,000"
