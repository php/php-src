--TEST--
ZE2 ArrayAccess and [] assignment
--FILE--
<?php

class OverloadedArray implements ArrayAccess {
    public $realArray;

    function __construct() {
        $this->realArray = array();
    }

    function offsetExists($index): bool {
        return array_key_exists($this->realArray, $index);
    }

    function offsetGet($index): mixed {
        return $this->realArray[$index];
    }

    function offsetSet($index, $value): void {
        if (is_null($index)) {
            $this->realArray[] = $value;
        } else {
            $this->realArray[$index] = $value;
        }
    }

    function offsetUnset($index): void {
        unset($this->realArray[$index]);
    }

    function dump() {
        var_dump($this->realArray);
    }
}

$a = new OverloadedArray;
$a[] = 1;
$a[1] = 2;
$a[2] = 3;
$a[] = 4;
$a->dump();
?>
--EXPECT--
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
