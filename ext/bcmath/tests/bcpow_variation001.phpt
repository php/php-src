--TEST--
bcpow() with a negative exponent
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcpow("2", "-4");
?>
--EXPECT--
0
