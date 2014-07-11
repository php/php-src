--TEST--
Bug #60217 (Requiring the same method from different traits and abstract methods have to be compatible, in both directions.)
--FILE--
<?php

trait TBroken1 {
    public abstract function foo($a, $b = 0);
}

trait TBroken2 {
    public abstract function foo($a);
}

class CBroken {
    use TBroken1, TBroken2;

    public function foo($a) {
        echo 'FOO';
    }
}

$o = new CBroken;
$o->foo(1);

--EXPECTF--
Fatal error: Declaration of TBroken2::foo($a) must be compatible with TBroken1::foo($a, $b = 0) in %s on line %d
