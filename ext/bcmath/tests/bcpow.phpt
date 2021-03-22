--TEST--
bcpow() function
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcpow("1", "2"),"\n";
echo bcpow("-2", "5", 4),"\n";
echo bcpow("2", "64"),"\n";
echo bcpow("-2.555", "5", 2),"\n";
?>
--EXPECT--
1
-32.0000
18446744073709551616
-108.88
