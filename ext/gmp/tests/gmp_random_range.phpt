--TEST--
gmp_random_range() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

echo "Done\n";
?>
--EXPECTF--
Done
