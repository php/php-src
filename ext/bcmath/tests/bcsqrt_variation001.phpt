--TEST--
bcsqrt() with argument of 0
--SKIPIF--
<?php if(!extension_loaded("bcmath")) die("skip bcmath extension not loaded"); ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcsqrt("0");
?>
--EXPECT--
0