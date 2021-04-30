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
Fatal error: Type of B::$a must be supertype of int|float (as in class A) in %s on line %d
