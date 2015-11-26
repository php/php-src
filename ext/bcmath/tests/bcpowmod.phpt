--TEST--
bcpowmod() - Raise an arbitrary precision number to another, reduced by a specified modulus
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcpowmod("5", "2", "7") . "\n";
echo bcpowmod("-2", "5", "7") . "\n";
echo bcpowmod("10", "2147483648", "2047");
?>
--EXPECT--
4
-4
790