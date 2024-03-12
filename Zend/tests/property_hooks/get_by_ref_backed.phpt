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
        set => $this->_prop = $value;
    }
}

?>
--EXPECTF--
Fatal error: Only virtual get hooks may return by reference in %s on line %d
