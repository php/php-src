--TEST--
Final property with private set
--FILE--
<?php

class A {
    final public $prop { get; private set; }
}

class B extends A {
    public $prop;
}

?>
--EXPECTF--
Fatal error: Cannot override final property B::$prop in %s on line %d
