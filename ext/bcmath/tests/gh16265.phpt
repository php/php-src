--TEST--
GH-16265 Segmentation fault (index oob) in ext/bcmath/libbcmath/src/convert.c:155
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcdiv('-0.01', -12.3456789000e10, 9);
?>
--EXPECT--
0.000000000
