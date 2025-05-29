--TEST--
Testing call_user_func() with autoload and passing invalid params
--FILE--
<?php

spl_autoload_register(function ($class) {
    var_dump($class);
});

try {
    call_user_func(array('foo', 'bar'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    call_user_func(array('', 'bar'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    call_user_func(array($foo, 'bar'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    call_user_func(array($foo, ''));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
string(3) "foo"
call_user_func(): Argument #1 ($callback) must be a valid callback, class "foo" not found
call_user_func(): Argument #1 ($callback) must be a valid callback, class "" not found

Warning: Undefined variable $foo in %s on line %d
call_user_func(): Argument #1 ($callback) must be a valid callback, first array member is not a valid class name or object

Warning: Undefined variable $foo in %s on line %d
call_user_func(): Argument #1 ($callback) must be a valid callback, first array member is not a valid class name or object
