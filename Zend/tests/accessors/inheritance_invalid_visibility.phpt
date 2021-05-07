--TEST--
Accessor visibility cannot be reduced during inheritance
--FILE--
<?php

class A {
    public $prop {
        get {}
        protected set {}
    }
}

class B extends A {
    public $prop {
        get {}
        private set {}
    }
}

?>
--EXPECTF--
Fatal error: Visibility of B::$prop::set() must be protected or higher (as in class A) in %s on line %d
