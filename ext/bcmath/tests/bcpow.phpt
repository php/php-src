--TEST--
bcpow() function
--SKIP--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcpow("1", "2"),"\n";
echo bcpow("-2", "5", 4),"\n";
echo bcpow("2", "64"),"\n";
?>
--EXPECT--
1
-32
18446744073709551616
