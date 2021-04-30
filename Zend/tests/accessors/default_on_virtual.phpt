--TEST--
Purely virtual accessors cannot have default value
--FILE--
<?php

class Test {
    private $_prop;
    public $prop = 0 {
        get { return $this->_prop; }
        set { $this->_prop = $value; }
    }
}

?>
--EXPECTF--
Fatal error: Cannot specify default value for property with explicit accessors in %s on line %d
