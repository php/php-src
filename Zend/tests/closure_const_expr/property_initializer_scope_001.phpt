--TEST--
Closure in property initializer may access private variables
--FILE--
<?php

class C {
    public Closure $d = static function (C $c) {
        echo $c->secret, PHP_EOL;
    };

    public function __construct(
        private string $secret,
    ) {}
}

$c = new C('secret');
($c->d)($c);


?>
--EXPECTF--
secret
