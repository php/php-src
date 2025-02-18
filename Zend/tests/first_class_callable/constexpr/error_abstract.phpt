--TEST--
FCC in initializer errors for FCC on abstract method
--FILE--
<?php

abstract class Foo {
    abstract public static function myMethod(string $foo);
}

const Closure = Foo::myMethod(...);

var_dump(Closure);
(Closure)("abc");

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot call abstract method Foo::myMethod() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
