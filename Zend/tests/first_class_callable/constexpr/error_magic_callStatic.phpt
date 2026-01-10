--TEST--
FCC in initializer errors for FCC on __callStatic() fallback.
--FILE--
<?php

class Foo {
    public static function __callStatic(string $name, array $foo) {
        echo "Called ", __METHOD__, "({$name})", PHP_EOL;
        var_dump($foo);
    }
}

const Closure = Foo::myMethod(...);

var_dump(Closure);
(Closure)("abc");

?>
--EXPECTF--
Fatal error: Uncaught Error: Creating a callable for the magic __callStatic() method is not supported in constant expressions in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
