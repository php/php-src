--TEST--
Class with abstract property hook not declared abstract (inherited 2)
--FILE--
<?php

abstract class A {
    public abstract $prop { get; }
}

class B extends A {
}

?>
--EXPECTF--
Fatal error: Class B contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (A::$prop::get) in %s on line %d
