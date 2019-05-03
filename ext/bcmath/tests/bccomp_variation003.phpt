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
echo bccomp("!@#$%^&*()_+", "!@@#$%^&*(()");
?>
--EXPECT--
0
0
0
