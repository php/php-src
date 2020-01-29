--TEST--
call_user_func() behavior when passing literal to reference parameter
--FILE--
<?php

namespace Foo;

var_dump(call_user_func('sort', []));
var_dump(\call_user_func('sort', []));

?>
--EXPECTF--
Warning: sort() expects argument #1 ($arg) to be passed by reference, value given in %s on line %d
bool(true)

Warning: sort() expects argument #1 ($arg) to be passed by reference, value given in %s on line %d
bool(true)
