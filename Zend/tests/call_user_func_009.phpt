--TEST--
call_user_func() behavior when passing literal to reference parameter
--FILE--
<?php

namespace Foo;

var_dump(call_user_func('sort', []));
var_dump(\call_user_func('sort', []));

?>
--EXPECTF--
Warning: Parameter 1 to sort() expected to be a reference, value given in %s on line %d
bool(true)

Warning: Parameter 1 to sort() expected to be a reference, value given in %s on line %d
bool(true)
