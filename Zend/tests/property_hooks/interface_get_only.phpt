--TEST--
Interface may contain only set with no implementation
--FILE--
<?php

interface I {
    public $prop { set; }
}

class A implements I {
    public $prop;
}

?>
--EXPECTF--
