--TEST--
FCC in property initializer may access private methods.
--FILE--
<?php

class C {
    public Closure $d = C::myMethod(...);

    private static function myMethod(string $foo) {
        echo "Called ", __METHOD__, PHP_EOL;
        var_dump($foo);
    }
}

$c = new C();
var_dump($c->d);
var_dump(($c->d)("abc"));

?>
--EXPECTF--
object(Closure)#%d (2) {
  ["function"]=>
  string(11) "C::myMethod"
  ["parameter"]=>
  array(1) {
    ["$foo"]=>
    string(10) "<required>"
  }
}
Called C::myMethod
string(3) "abc"
NULL
