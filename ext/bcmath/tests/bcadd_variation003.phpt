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
--EXPECTF--
Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
2

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
-2

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
2

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
-2

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
2

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
-2

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
2

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
-2
