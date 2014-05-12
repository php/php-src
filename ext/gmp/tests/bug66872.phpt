--TEST--
Bug #66872: Crash when passing string to gmp_testbit
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_testbit("abc", 1));

?>
--EXPECTF--
Warning: gmp_testbit(): Unable to convert variable to GMP - string is not an integer in %s on line %d
bool(false)
