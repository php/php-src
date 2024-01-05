--TEST--
Validating signatures involving generic parameters
--FILE--
<?php

abstract class Test<T> {
    public function method(T $param): T {}
}

class Test2<T1, T2> extends Test<T1> {
    public function method(T2 $param): T2 {}
}

?>
--EXPECTF--
Fatal error: Declaration of Test2::method(T2 $param): T2 must be compatible with Test::method(T $param): T in %s on line %d
