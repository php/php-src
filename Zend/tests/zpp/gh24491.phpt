--TEST--
GH-24491: Callable error string leaks when an exception is thrown during zpp
--FILE--
<?php
set_error_handler(function (int $errno, string $errstr) {
    throw new Exception($errstr);
});

class A {
    public static function foo() {}

    public static function test() {
        register_shutdown_function(['self', 'foo']);
    }
}

try {
    A::test();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Use of "self" in callables is deprecated
