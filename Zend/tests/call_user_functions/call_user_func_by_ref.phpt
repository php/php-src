--TEST--
call_user_func() with ref arg and type check
--FILE--
<?php

function test(Type &$ref) {
}
try {
    call_user_func('test', 0);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: test(): Argument #1 ($ref) must be passed by reference, value given in %s on line %d
test(): Argument #1 ($ref) must be of type Type, int given, called in %s on line %d
