--TEST--
ZE2 ArrayAccess and ASSIGN_OP operators (+=)
--FILE--
<?php

class OverloadedArray implements ArrayAccess {
    public $realArray;

    function __construct() {
        $this->realArray = array(1,2,3);
    }

    function offsetExists($index) {
        return array_key_exists($this->realArray, $index);
    }

    function offsetGet($index) {
        return $this->realArray[$index];
    }

    function offsetSet($index, $value) {
        $this->realArray[$index] = $value;
    }

    function offsetUnset($index) {
        unset($this->realArray[$index]);
    }
}

$a = new OverloadedArray;
$a[1] += 10;
var_dump($a[1]);
echo "---Done---\n";
?>
--EXPECT--
int(12)
---Done---
