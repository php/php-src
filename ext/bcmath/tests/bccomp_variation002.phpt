--TEST--
bccomp() with negative value
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
echo bccomp("-2", "-2") . "\n";
echo bccomp("-2", "2", "1") . "\n";
echo bccomp("-2.29", "-2.3", "2") . "\n";
echo bccomp("-2.29", "2.3", "2") . "\n";
echo bccomp("2.29", "-2.3", "2") . "\n";
echo bccomp("-2.29", "-2.3", "1") . "\n";
echo bccomp("-2.29", "0", "1") . "\n";
?>
--EXPECT--
0
-1
1
-1
1
1
-1
