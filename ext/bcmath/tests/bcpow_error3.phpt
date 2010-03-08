--TEST--
bcpow() incorrect argument count
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcpow();
?>
--EXPECTF--
Warning: Wrong parameter count for bcpow() in %s on line %d