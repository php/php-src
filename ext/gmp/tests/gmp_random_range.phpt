--TEST--
gmp_random_range() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_random_range(1, -1));

echo "Done\n";
?>
--EXPECTF--
Warning: gmp_random_range(): The minimum value must be lower than the maximum value in %s on line %d
NULL
Done
