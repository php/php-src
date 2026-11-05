--TEST--
Child constructors are checked against all parent interface constructors
--FILE--
<?php

interface A {
    public function __construct(int|float $param);
}
interface B {
    public function __construct(int $param);
}
class X implements A, B {
    public function __construct(int|float $param) {}
}
class Y extends X {
    public function __construct(int $param) {}
}
new Y(42);

?>
--EXPECTF--
Fatal error: Declaration of Y::__construct(int $param) must be compatible with A::__construct(int|float $param) in %s on line %d
