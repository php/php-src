--TEST--
Interfaces don't set prototypes to their parent method
--XFAIL--
X::__constructor()'s prototype is set to B::__construct(). Y::__construct() then
uses prototype to verify LSP, but misses A::__construct() which has a stricter
signature.
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
