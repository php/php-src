--TEST--
Abstract private trait method not implemented
--FILE--
<?php

trait T {
    abstract private function method(int $a, string $b);
}

abstract class C {
    use T;
}

class D extends C {
    private function method(int $a, string $b) {}
}

?>
--EXPECTF--
Fatal error: Class C must implement 1 abstract private method (C::method) in %s on line %d
