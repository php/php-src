--TEST--
Final hooks
--FILE--
<?php

class A {
    public $prop {
        final get { return 42; }
    }
}

class B extends A {
    public $prop {
        get { return 24; }
    }
}

?>
--EXPECTF--
Fatal error: Cannot override final property hook A::$prop::get() in %s on line %d
