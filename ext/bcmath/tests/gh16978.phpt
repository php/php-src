--TEST--
GH-16978 Stack buffer overflow ext/bcmath/libbcmath/src/div.c:464:12 in bc_divide
--EXTENSIONS--
bcmath
--FILE--
<?php
echo bcpow('10', '-112', 10) . "\n";
echo bcdiv('1', '10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000', 1);
?>
--EXPECT--
0.0000000000
0.0
