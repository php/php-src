--TEST--
bccomp() function
--SKIP--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--FILE--
<?php
echo bccomp("-1", "5", 4),"\n";
echo bccomp("1928372132132819737213", "8728932001983192837219398127471"),"\n";
echo bccomp("1.00000000000000000001", "1", 2),"\n";
echo bccomp("97321", "2321"),"\n";
?>
--EXPECT--
-1
-1
0
1
