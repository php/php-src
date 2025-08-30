--TEST--
FCC in initializer warns for static reference to trait method.
--FILE--
<?php

trait Foo {
    public static function myMethod(string $foo) {
        echo "Called ", __METHOD__, PHP_EOL;
        var_dump($foo);
    }
}

const Closure = Foo::myMethod(...);

var_dump(Closure);
(Closure)("abc");

?>
--EXPECTF--
Deprecated: Calling static trait method Foo::myMethod is deprecated, it should only be called on a class using the trait in %s on line %d
object(Closure)#%d (2) {
  ["function"]=>
  string(13) "Foo::myMethod"
  ["parameter"]=>
  array(1) {
    ["$foo"]=>
    string(10) "<required>"
  }
}
Called Foo::myMethod
string(3) "abc"
