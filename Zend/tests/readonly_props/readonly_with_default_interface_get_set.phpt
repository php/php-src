--TEST--
Readonly public(set) property with default value does not satisfy get/set interface property
--FILE--
<?php

interface I {
    public int $prop { get; set; }
}

// does not satisfy set
class C implements I {
    public public(set) readonly int $prop = 42;
}
?>
--EXPECTF--
Fatal error: Class C contains 1 abstract method and must therefore be declared abstract or implement the remaining method (I::$prop::set) in %s on line %d
