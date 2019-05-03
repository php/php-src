--TEST--
bcadd() with non-numeric values and numeric values
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bcadd("eeee", "2")."\n";
echo bcadd("eeee", "-2")."\n";
echo bcadd("2", "%%%$^&&*(*())")."\n";
echo bcadd("-2", "%%%$^&&*(*())")."\n";
echo bcadd("2.1", "~!@@##^*()_")."\n";
echo bcadd("-2.1", "~!@@##^*()_")."\n";
echo bcadd("~!@@##^*()_", "2.1")."\n";
echo bcadd("~!@@##^*()_", "-2.1")."\n";
?>
--EXPECT--
2
-2
2
-2
2
-2
2
-2
