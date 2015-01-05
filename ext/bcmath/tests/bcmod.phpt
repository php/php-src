--TEST--
bcmod() function
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcmod("11", "2"),"\n";
echo bcmod("-1", "5"),"\n";
echo bcmod("8728932001983192837219398127471", "1928372132132819737213"),"\n";
?>
--EXPECT--
1
-1
1459434331351930289678
