--TEST--
bcadd() with non-integers
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=5
--FILE--
<?php
echo bcadd("2.2", "4.3", "2")."\n";
echo bcadd("2.2", "-7.3", "1")."\n";
echo bcadd("-4.27", "7.3");
?>
--EXPECTF--
6.50
-5.1
3.03000