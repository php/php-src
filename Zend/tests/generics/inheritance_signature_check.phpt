--TEST--
Validating signatures involving generic parameters
--FILE--
<?php

abstract class Test<T> {
    public function method(T $param) {}
}

class Test2 extends Test<int> {
    public function method(string $param) {}
}

?>
--EXPECTF--
Fatal error: Declaration of Test2::method(string $param) must be compatible with Test::method(T $param) in %s on line %d
