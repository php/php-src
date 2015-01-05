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
Warning: bcadd() expects at least 2 parameters, 0 given in %s on line %d