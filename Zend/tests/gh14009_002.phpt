--TEST--
GH-14009: Traits inherit prototype
--FILE--
<?php

class P {
    protected function common() {}
}

class A extends P {}

trait T {
    private abstract function common(int $param);
}

class B extends P {
    use T;
}

?>
--EXPECTF--
Fatal error: Declaration of P::common() must be compatible with T::common(int $param) in %s on line %d
