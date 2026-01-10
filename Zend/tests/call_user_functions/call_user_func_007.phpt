--TEST--
call_user_func() should not use FUNC_ARG fetches
--FILE--
<?php

function foo(&$ref) { $ref = 24; }

$a = [];
call_user_func('foo', $a[0][0]);
var_dump($a);

?>
--EXPECTF--
Warning: Undefined array key 0 in %s on line %d

Warning: Trying to access array offset on null in %s on line %d

Warning: foo(): Argument #1 ($ref) must be passed by reference, value given in %s on line %d
array(0) {
}
