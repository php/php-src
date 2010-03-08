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
Warning: Wrong parameter count for bcmul() in %s on line %d