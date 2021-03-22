--TEST--
bccomp() with non-integers
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
echo bccomp("2.2", "2.2", "2")."\n";
echo bccomp("2.32", "2.2", "2")."\n";
echo bccomp("2.29", "2.3", "2");
?>
--EXPECT--
0
1
-1
