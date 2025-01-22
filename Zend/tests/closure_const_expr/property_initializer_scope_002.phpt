--TEST--
Closure in property initializer may not access unrelated private variables
--FILE--
<?php

class C {
    public Closure $d = static function (E $e) {
        echo $e->secret, PHP_EOL;
    };

    
}

class E {
    public function __construct(
        private string $secret,
    ) {}
}

$c = new C();
($c->d)(new E('secret'));


?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access private property E::$secret in %s:%d
Stack trace:
#0 %s(%d): C::{closure:%s:%d}(Object(E))
#1 {main}
  thrown in %s on line %d
