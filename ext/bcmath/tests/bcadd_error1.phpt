--TEST--
bcadd() incorrect argument count
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcadd();
?>
--EXPECTF--
Warning: Wrong parameter count for bcadd() in %s on line %d