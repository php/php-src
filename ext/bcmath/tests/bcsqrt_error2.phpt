--TEST--
bcsqrt() incorrect argument count
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcsqrt();
?>
--EXPECTF--
Warning: Wrong parameter count for bcsqrt() in %s on line %d