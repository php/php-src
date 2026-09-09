--TEST--
GH-23630: Implementation must satisfy the most specific parent interface
--FILE--
<?php

interface A {
    public function __construct(?string $name = null);
}

interface B {
    public function __construct(?string $name = null, ?int $id = null);
}

interface C extends A, B {
}

class Test implements C {
    public function __construct(?string $name = null) {}
}

?>
--EXPECTF--
Fatal error: Declaration of Test::__construct(?string $name = null) must be compatible with B::__construct(?string $name = null, ?int $id = null) in %s on line %d
