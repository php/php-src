--TEST--
bccomp() with negative value
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bccomp("-2", "-2")."\n";
echo bccomp("-2", "2", "1")."\n";
echo bccomp("-2.29", "-2.3", "2");
?>
--EXPECT--
0
-1
1
