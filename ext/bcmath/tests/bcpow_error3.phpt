--TEST--
bcpow() incorrect argument count
--SKIPIF--
<?php if(!extension_loaded("bcmath")) die("skip bcmath extension not loaded"); ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcpow();
?>
--EXPECTF--
Warning: bcpow() expects at least 2 parameters, 0 given in %s on line %d