--TEST--
Class with abstract accessor not declared abstract (inherited 1)
--FILE--
<?php

abstract class A {
    public $prop { abstract get; set; }
}

class B extends A {
    public $prop { set {} }
}

?>
--EXPECTF--
Fatal error: Class B contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (A::$prop::get) in %s on line %d
