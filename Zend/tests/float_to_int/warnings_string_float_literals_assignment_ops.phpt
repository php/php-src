--TEST--
Implicit float to int conversions should warn for literals in combined assingment operetor
--FILE--
<?php

echo 'Bitwise ops:' . \PHP_EOL;

$var = 3;
$var |= '1.5';
var_dump($var);

$var = 3;
$var &= '1.5';
var_dump($var);

$var = 3;
$var ^= '1.5';
var_dump($var);

$var = 3;
$var <<= '1.5';
var_dump($var);

$var = 3;
$var >>= '1.5';
var_dump($var);

echo 'Modulo:' . \PHP_EOL;
$var = 9;
$var %= '2.5';
var_dump($var);

?>
--EXPECTF--
Bitwise ops:

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(3)

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(1)

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(2)

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(6)

Deprecated: Implicit conversion from float-string "1.5" to int loses precision in %s on line %d
int(1)
Modulo:

Deprecated: Implicit conversion from float-string "2.5" to int loses precision in %s on line %d
int(1)
