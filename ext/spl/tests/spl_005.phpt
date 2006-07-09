--TEST--
SPL: spl_object_hash()
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

var_dump(spl_object_hash(new stdClass));
var_dump(spl_object_hash(42));
var_dump(spl_object_hash());

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
string(32) "%s"

Warning: spl_object_hash() expects parameter 1 to be object, integer given in %sspl_005.php on line %d
NULL

Warning: spl_object_hash() expects exactly 1 parameter, 0 given in %sspl_005.php on line %d
NULL
===DONE===
