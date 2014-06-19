--TEST--
Bug #64106: Segfault on SplFixedArray[][x] = y when extended
--FILE--
<?php

class MyFixedArray extends SplFixedArray {
    public function offsetGet($offset) {}
}

$array = new MyFixedArray(10);
$array[][1] = 10;

?>
--EXPECTF--
Notice: Indirect modification of overloaded element of MyFixedArray has no effect in %s on line %d
