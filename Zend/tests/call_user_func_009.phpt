--TEST--
call_user_func() behavior when passing literal to reference parameter
--FILE--
<?php

namespace Foo;

var_dump(call_user_func('sort', []));
var_dump(\call_user_func('sort', []));

?>
--EXPECTF--
Warning: sort(): Argument #1 ($array) must be passed by reference, value given in %s on line %d
bool(true)

Warning: sort(): Argument #1 ($array) must be passed by reference, value given in %s on line %d
bool(true)
