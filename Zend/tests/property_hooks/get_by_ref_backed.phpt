--TEST--
Virtual get hook allows returning by reference
--FILE--
<?php

class A {
    public $prop;
}

class B extends A {
    private $_prop;
    public $prop {
        &get => $this->_prop;
        set { $this->_prop = $value; }
    }
}

?>
--EXPECTF--
Fatal error: Get hook of backed property B::prop with set hook may not return by reference in %s on line %d
