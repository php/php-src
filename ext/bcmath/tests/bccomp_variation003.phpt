--TEST--
bccomp() with non-numeric values
--SKIPIF--
<?php if(!extension_loaded("bcmath")) print "skip"; ?>
--INI--
bcmath.scale=0
--FILE--
<?php
echo bccomp("ssssssssss2", "!@@#$%^&*(()")."\n";
echo bccomp("1@#$%%%^&*", "!@@#$%^&*(()")."\n";
echo bccomp("!@#$%^&*()_+", "!@@#$%^&*(()")."\n";
?>
--EXPECTF--
Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d

Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
0

Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d

Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
0

Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d

Warning: bccomp(): bcmath function argument is not well-formed in %s on line %d
0
