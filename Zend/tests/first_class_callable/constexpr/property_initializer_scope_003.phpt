--TEST--
FCC in property initializer may access protected methods of parent.
--FILE--
<?php

class P {
    protected static function myMethod(string $foo) {
        echo "Called ", __METHOD__, PHP_EOL;
        var_dump($foo);
    }
}


class C extends P {
    public Closure $d = self::myMethod(...);
}

$c = new C();
var_dump($c->d);
var_dump(($c->d)("abc"));

?>
--EXPECTF--
object(Closure)#%d (2) {
  ["function"]=>
  string(11) "P::myMethod"
  ["parameter"]=>
  array(1) {
    ["$foo"]=>
    string(10) "<required>"
  }
}
Called P::myMethod
string(3) "abc"
NULL
