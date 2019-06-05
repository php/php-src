--TEST--
bccomp() with non-numeric values and numeric values
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bccomp("2", "!@@#$%^&*(()")."\n";
echo bccomp("!@@#$%^&*(()", "2")."\n";
echo bccomp("!@@#$%^&*(()", "-2")."\n";
echo bccomp("1@#$%%%^&*", "2.1")."\n";
echo bccomp("-2.1", "1@#$%%%^&*")."\n";
?>
--EXPECTF--
Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
1

Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
-1

Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
1

Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
-1

Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
-1
