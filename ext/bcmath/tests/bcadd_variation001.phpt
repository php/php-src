--TEST--
bcadd() with non-integers
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=5
--FILE--
<?php
echo bcadd("2.2", "4.3", "2")."\n";
echo bcadd("2.2", "4.3", 2)."\n";
echo bcadd("2.2", "-7.3", "1")."\n";
echo bcadd("2.2", "-7.3", 1)."\n";
echo bcadd("-4.27", "7.3")."\n";
echo bcadd("-4.27", "-7.3");
?>
--EXPECT--
6.50
6.50
-5.1
-5.1
3.03000
-11.57000
