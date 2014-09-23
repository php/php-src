--TEST--
gmp_random_bits() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_random_bits(0));
var_dump(gmp_random_bits(-1));

echo "Done\n";
?>
--EXPECTF--
Warning: gmp_random_bits(): The number of bits must be positive in %s on line %d
NULL

Warning: gmp_random_bits(): The number of bits must be positive in %s on line %d
NULL
Done
