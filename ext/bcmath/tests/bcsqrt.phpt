--TEST--
bcsqrt() function
--SKIP--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--FILE--
<?php
echo bcsqrt("9"),"\n";
echo bcsqrt("1928372132132819737213", 5),"\n";
?>
--EXPECT--
3
43913234134.28826
