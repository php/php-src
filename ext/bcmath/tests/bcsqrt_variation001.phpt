--TEST--
bcsqrt() with argument of 0
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcsqrt("0");
?>
--EXPECTF--
0
