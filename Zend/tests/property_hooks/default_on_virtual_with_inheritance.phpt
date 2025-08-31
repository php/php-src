--TEST--
Virtual property cannot have default value
--FILE--
<?php

class A {
    public $prop { get {} set {} }
}

class B extends A {
    public $prop = 0 {
        get {}
        set {}
    }
}

?>
--EXPECTF--
Fatal error: Cannot specify default value for virtual hooked property B::$prop in %s on line %d
