--TEST--
bcmul() function
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcmul("1", "2"),"\n";
echo bcmul("-3", "5"),"\n";
echo bcmul("1234567890", "9876543210"),"\n";
echo bcmul("2.5", "1.5", 2),"\n";
?>
--EXPECT--
2
-15
12193263111263526900
3.75
