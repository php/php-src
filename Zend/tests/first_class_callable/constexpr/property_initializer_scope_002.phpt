--TEST--
FCC in property initializer may not access unrelated private methods.
--FILE--
<?php

class E {
    private static function myMethod(string $foo) {
        echo "Called ", __METHOD__, PHP_EOL;
        var_dump($foo);
    }
}

class C {
  public Closure $d = E::myMethod(...);
}

$c = new C();
var_dump($c->d);
var_dump(($c->d)("abc"));

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private method E::myMethod() from scope C in %s:%d
Stack trace:
#0 %s(%d): [constant expression]()
#1 {main}
  thrown in %s on line %d
