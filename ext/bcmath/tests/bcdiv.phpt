--TEST--
bcdiv() function
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcdiv("1", "2"),"\n";
echo bcdiv("1", "2", 2),"\n";
echo bcdiv("-1", "5", 4),"\n";
echo bcdiv("8728932001983192837219398127471", "1928372132132819737213", 2),"\n";
?>
--EXPECT--
0
0.50
-0.2000
4526580661.75
