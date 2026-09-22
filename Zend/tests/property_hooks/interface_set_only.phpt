--TEST--
Interface may contain only get with no implementation
--FILE--
<?php

interface I {
    public $prop { get; }
}

class A implements I {
    public $prop { get {} }
}

?>
--EXPECTF--
