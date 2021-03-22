--TEST--
Invalid type compatibility for read-only property
--FILE--
<?php

class A {
    public int $a { get; }
}
class B extends A {
    public int|float $a { get; }
}

?>
--EXPECTF--
Fatal error: Type of B::$a must be subtype of int (as in class A) in %s on line %d
