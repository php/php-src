--TEST--
Allow defining FCC for static methods in const expressions.
--FILE--
<?php

class Foo {
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
