--TEST--
FCC in initializer errors for FCC on instance call.
--FILE--
<?php

class Foo {
    public function myMethod(string $foo) {
        echo "Called ", __METHOD__, PHP_EOL;
        var_dump($foo);
    }
}

const Closure = (new Foo())->myMethod(...);

var_dump(Closure);
(Closure)("abc");

?>
--EXPECTF--
Fatal error: Constant expression contains invalid operations in %s on line %d
