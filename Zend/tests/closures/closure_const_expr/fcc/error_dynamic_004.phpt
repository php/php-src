--TEST--
FCC in initializer errors for FCC on 'static::'.
--FILE--
<?php

class Foo {
    public const Closure = static::myMethod(...);

    public static function myMethod(string $foo) {
        echo "Called ", __METHOD__, PHP_EOL;
        var_dump($foo);
    }
}

var_dump(Foo::Closure);
(Foo::Closure)("abc");

?>
--EXPECTF--
Fatal error: "static" is not allowed in compile-time constants in %s on line %d
