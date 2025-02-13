--TEST--
GH-17728: Assertion failure when calling static method of trait with `__callStatic()` with throwing error handler
--FILE--
<?php

set_error_handler(function (int $errno, string $errstr, string $errfile, int $errline) {
    throw new \ErrorException($errstr, 0, $errno, $errfile, $errline);
});

trait Foo {
    public static function __callStatic($method, $args) {
        var_dump($method);
    }
}

try {
    Foo::bar();
} catch (ErrorException $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Calling static trait method Foo::bar is deprecated, it should only be called on a class using the trait
