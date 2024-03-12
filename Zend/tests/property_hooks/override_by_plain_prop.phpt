--TEST--
Hooks must not be overridden by a plain property
--FILE--
<?php

class A {
    public $prop {
        get { echo __METHOD__, "\n"; return 42; }
        set { echo __METHOD__, "\n"; }
    }
}

class B extends A {
    public $prop;
}

?>
--EXPECTF--
Fatal error: Non-virtual property B::$prop must not redeclare virtual property A::$prop in %s on line %d
