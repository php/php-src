--TEST--
bcsub() function
--SKIP--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--FILE--
<?php
echo bcsub("1", "2"),"\n";
echo bcsub("-1", "5", 4),"\n";
echo bcsub("8728932001983192837219398127471", "1928372132132819737213", 2),"\n";
?>
--EXPECT--
-1
-6.0000
8728932000054820705086578390258.00
