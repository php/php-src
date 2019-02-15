--TEST--
bcadd() incorrect argument count
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=-2
--FILE--
<?php
echo bcadd("-4.27", "7.3");
?>
--EXPECT--
3
