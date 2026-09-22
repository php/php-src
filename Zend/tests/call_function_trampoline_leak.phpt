--TEST--
zend_call_function() must release the trampoline when it bails before the call
--FILE--
<?php

class A {
    #[\Deprecated]
    public function __call($func, $args) {
    }
}

set_error_handler(function ($errno, $errstr) {
    throw new Exception($errstr);
});

// Indirect call so that the callable is resolved by zend_call_function()
// rather than by the ZEND_INIT_USER_CALL handler, which frees the trampoline
// itself.
$call = 'call_user_func_array';

try {
    $call([new A(), 'foo'], []);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $call([Closure::fromCallable('strcmp'), '__invoke'], ['string2' => 'b']);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: Method A::foo() is deprecated
ArgumentCountError: Closure::__invoke(): Argument #1 ($string1) not passed
