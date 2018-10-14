--TEST--
bcmod() - mod by 0
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcmod("10", "0");
?>
--EXPECTF--
Warning: bcmod(): Division by zero in %s on line %d
