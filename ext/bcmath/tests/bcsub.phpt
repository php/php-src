--TEST--
bcsub() function
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcsub("1", "2"),"\n";
echo bcsub("-1", "5", 4),"\n";
echo bcsub("1.555", "2.555", 2),"\n";
echo bcsub("8728932001983192837219398127471", "1928372132132819737213", 2),"\n";
?>
--EXPECT--
-1
-6.0000
-1.00
8728932000054820705086578390258.00
