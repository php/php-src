--TEST--
By-ref return mismatch during inheritance
--FILE--
<?php

class A {
    public $prop { &get; set; }
}
class B extends A {
    public $prop { get; }
}

?>
--EXPECTF--
Fatal error: B::$prop::get() must return by reference (as in class A) in %s on line %d
