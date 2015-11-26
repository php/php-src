--TEST--
bcscale() function
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcadd("1", "2"),"\n";
bcscale(2);
echo bcadd("1", "2"),"\n";
bcscale(10);
echo bcadd("1", "2"),"\n";
bcscale(0);
echo bcadd("1", "2"),"\n";
?>
--EXPECT--
3
3.00
3.0000000000
3
