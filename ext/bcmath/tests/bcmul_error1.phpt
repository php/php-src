--TEST--
bcmul() incorrect argument count
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcmul();
?>
--EXPECTF--
Warning: bcmul() expects at least 2 parameters, 0 given in %s on line %d