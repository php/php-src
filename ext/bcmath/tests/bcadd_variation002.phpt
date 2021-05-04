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
--EXPECTF--
Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
0

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
0

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d

Warning: bcadd(): bcmath function argument is not well-formed in %s on line %d
0
