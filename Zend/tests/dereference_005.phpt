--TEST--
Testing array dereference on object that implements ArrayAccess
--FILE--
<?php

error_reporting(E_ALL);

class obj implements arrayaccess {
    private $container = array();
    public function __construct() {
        $this->container = array(
            "one"   => 1,
            "two"   => 2,
            "three" => 3,
        );
    }
    public function offsetSet($offset, $value): void {
        $this->container[$offset] = $value;
    }
    public function offsetExists($offset): bool {
        return isset($this->container[$offset]);
    }
    public function offsetUnset($offset): void {
        unset($this->container[$offset]);
    }
    public function offsetGet($offset): mixed {
        return isset($this->container[$offset]) ? $this->container[$offset] : null;
    }
}

function x() {
    return new obj;
}
var_dump(x()['two']);

?>
--EXPECT--
int(2)
