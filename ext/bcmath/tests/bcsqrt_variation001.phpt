--TEST--
bcsqrt() with argument of 0
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcsqrt("0");
?>
--EXPECT--
0
