--TEST--
Property itself may be marked final (hook)
--FILE--
<?php

class A {
    public final $prop { get {} set {} }
}

class B extends A {
    public $prop { get {} set {} }
}

?>
--EXPECTF--
Fatal error: Cannot override final property A::$prop in %s on line %d
