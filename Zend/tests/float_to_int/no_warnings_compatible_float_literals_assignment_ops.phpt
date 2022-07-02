--TEST--
Implicit float to int conversions should not warn for literals in combined assingment operetor if float has a fractional part equal to 0
--FILE--
<?php

echo 'Bitwise ops:' . \PHP_EOL;

$var = 3;
$var |= 1.0;
var_dump($var);

$var = 3;
$var &= 1.0;
var_dump($var);

$var = 3;
$var ^= 1.0;
var_dump($var);

$var = 3;
$var <<= 1.0;
var_dump($var);

$var = 3;
$var >>= 1.0;
var_dump($var);

echo 'Modulo:' . \PHP_EOL;
$var = 9;
$var %= 2.0;
var_dump($var);

?>
--EXPECT--
Bitwise ops:
int(3)
int(1)
int(2)
int(6)
int(1)
Modulo:
int(1)
