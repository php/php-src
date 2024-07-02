--TEST--
Invalid type compatibility for read-only property
--FILE--
<?php

class A {
    public int $a { get {} }
}
class B extends A {
    public int|float $a { get {} }
}

?>
--EXPECTF--
Fatal error: Declaration of B::$a::get(): int|float must be compatible with A::$a::get(): int in %s on line %d
