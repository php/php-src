--TEST--
FCC in initializer errors for static reference to instance method.
--FILE--
<?php

class Foo {
    public function myMethod(string $foo) {
        echo "Called ", __METHOD__, PHP_EOL;
        var_dump($foo);
    }
}

const Closure = Foo::myMethod(...);

var_dump(Closure);
(Closure)("abc");

?>
--EXPECTF--
Fatal error: Uncaught Error: Failed to create closure from non-static method Foo::myMethod() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
