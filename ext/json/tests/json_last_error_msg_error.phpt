--TEST--
json_last_error_msg() failures
--SKIPIF--
<?php !extension_loaded('json') && die('skip json extension not available') ?>
--FILE--
<?php

var_dump(json_last_error_msg());
var_dump(json_last_error_msg(TRUE));
var_dump(json_last_error_msg('some', 4, 'args', 'here'));

?>
--EXPECTF--
string(8) "No error"

Warning: json_last_error_msg() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: json_last_error_msg() expects exactly 0 parameters, 4 given in %s on line %d
NULL

