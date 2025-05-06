--TEST--
Interfaces don't set prototypes to their parent method
--FILE--
<?php

interface A {
    public function __construct(int $param);
}
interface B extends A {
    public function __construct(int|float $param);
}
class Test implements B {
    public function __construct(int $param) {}
}
new Test(42);

?>
--EXPECTF--
Fatal error: Declaration of Test::__construct(int $param) must be compatible with B::__construct(int|float $param) in %s on line %d
