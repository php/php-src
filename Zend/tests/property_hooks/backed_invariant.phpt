--TEST--
Backed property is invariant
--FILE--
<?php

class A {
    public string|int $prop { get => $this->prop; }
}

class B extends A {
    public string $prop { get => 'foo'; }
}

?>
--EXPECTF--
Fatal error: Type of B::$prop must be string|int (as in class A) in %s on line %d
