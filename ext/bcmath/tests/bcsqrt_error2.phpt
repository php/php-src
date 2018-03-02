--TEST--
bcsqrt() incorrect argument count
--SKIPIF--
<?php if(!extension_loaded("bcmath")) die("skip bcmath extension not loaded"); ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcsqrt();
?>
--EXPECTF--
Warning: bcsqrt() expects at least 1 parameter, 0 given in %s on line %d