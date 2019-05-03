--TEST--
bcadd() with non-numeric values
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcadd("eeee", "eeeefff")."\n";
echo bcadd("eeee", "%%%$^&&*(*())")."\n";
echo bcadd("eeee", "~!@@##^*()_")."\n";
?>
--EXPECT--
0
0
0
