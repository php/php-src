--TEST--
Invalid type compatibility for write-only property
--FILE--
<?php

class A {
    public int|float $a { set {} }
}
class B extends A {
    public int $a { set {} }
}

?>
--EXPECTF--
Fatal error: Declaration of B::$a::set(int $value): void must be compatible with A::$a::set(int|float $value): void in %s on line %d
