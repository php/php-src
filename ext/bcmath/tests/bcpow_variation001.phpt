--TEST--
bcpow() with a negative exponent
--SKIPIF--
<?php if(!extension_loaded("bcmath")) die("skip bcmath extension not loaded"); ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcpow("2", "-4");
?>
--EXPECT--
0