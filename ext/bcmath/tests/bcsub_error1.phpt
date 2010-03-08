--TEST--
bcsub() incorrect argument count
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcsub();
?>
--EXPECTF--
Warning: Wrong parameter count for bcsub() in %s on line %d