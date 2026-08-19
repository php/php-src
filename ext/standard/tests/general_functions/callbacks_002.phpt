--TEST--
call_user_func(): Wrong parameters
--FILE--
<?php

try {
    call_user_func(array('Foo', 'bar'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    call_user_func(array(NULL, 'bar'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    call_user_func(array('stdclass', NULL));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, class "Foo" not found
TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, first array member is not a valid class name or object
TypeError: call_user_func(): Argument #1 ($callback) must be a valid callback, second array member is not a valid method
