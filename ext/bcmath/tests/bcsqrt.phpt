--TEST--
bcsqrt() function
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcsqrt("9"),"\n";
echo bcsqrt("9.444", 2),"\n";
echo bcsqrt("1928372132132819737213", 5),"\n";
echo bcsqrt("0.5", 5), "\n";
?>
--EXPECT--
3
3.07
43913234134.28826
0.70710
