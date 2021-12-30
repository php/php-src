--TEST--
bcadd() function
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcadd("1", "2"),"\n";
echo bcadd("-1", "5", 4),"\n";
echo bcadd("-1", "-5", 4),"\n";
echo bcadd("1", "-5", 4),"\n";
echo bcadd("1928372132132819737213", "8728932001983192837219398127471", 2),"\n";
?>
--EXPECT--
3
4.0000
-6.0000
-4.0000
8728932003911564969352217864684.00
