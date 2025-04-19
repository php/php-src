--TEST--
Repeated associated type
--FILE--
<?php

interface I {
    type T;
    type T;
    public function foo(T $param): T;
}

class C implements I {
    public function foo(string $param): string {}
}

?>
--EXPECTF--
Fatal error: Cannot have two associated types with the same name "T" in %s on line %d
