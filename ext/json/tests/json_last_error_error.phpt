--TEST--
json_last_error() failures
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_last_error());
var_dump(json_last_error(true));
var_dump(json_last_error('some', 4, 'args', 'here'));

?>
--EXPECTF--
int(0)

Warning: json_last_error() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: json_last_error() expects exactly 0 parameters, 4 given in %s on line %d
NULL
