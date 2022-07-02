--TEST--
bcpowmod() - Raise an arbitrary precision number to another, reduced by a specified modulus
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
var_dump(bcpowmod("5", "2", "7"));
var_dump(bcpowmod("-2", "5", "7"));
var_dump(bcpowmod("10", "2147483648", "2047"));
var_dump(bcpowmod("10", "0", "2047"));
?>
--EXPECT--
string(1) "4"
string(2) "-4"
string(3) "790"
string(1) "1"
