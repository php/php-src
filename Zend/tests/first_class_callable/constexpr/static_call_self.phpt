--TEST--
Allow defining FCC for static methods referenced by 'self::' in const expressions.
--FILE--
<?php

class Foo {
    public const Closure = self::myMethod(...);

    public static function myMethod(string $foo) {
        echo "Called ", __METHOD__, PHP_EOL;
        var_dump($foo);
    }
}

var_dump(Foo::Closure);
(Foo::Closure)("abc");

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
