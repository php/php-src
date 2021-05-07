--TEST--
Cannot override plain property with accessor property
--FILE--
<?php

class A {
    public $prop;
}
class B extends A {
    public $prop { get; set; }
}

?>
--EXPECTF--
Fatal error: Cannot override plain property A::$prop with accessor property in class B in %s on line %d
