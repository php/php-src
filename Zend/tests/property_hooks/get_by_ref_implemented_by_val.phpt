--TEST--
By-reference get may be not implemented as by-value
--FILE--
<?php

interface I {
    public $prop { &get; }
}

class A implements I {
    public $prop {
        get => $this->prop;
    }
}

?>
--EXPECTF--
Fatal error: Declaration of A::$prop::get() must be compatible with & I::$prop::get() in %s on line %d
