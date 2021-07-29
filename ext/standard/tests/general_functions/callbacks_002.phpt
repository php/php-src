--TEST--
call_user_func(): Wrong parameters
--FILE--
<?php

try {
    call_user_func(array('Foo', 'bar'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    call_user_func(array(NULL, 'bar'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    call_user_func(array('stdclass', NULL));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
call_user_func(): Argument #1 ($callback) must be a valid callback, class "Foo" not found
call_user_func(): Argument #1 ($callback) must be a valid callback, first array member is not a valid class name or object
call_user_func(): Argument #1 ($callback) must be a valid callback, second array member is not a valid method
