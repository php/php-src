--TEST--
Overriding SplFixedArray::offsetGet() only
--FILE--
<?php

class MyArray extends SplFixedArray {
    public function offsetGet($key) {
        return "prefix_" . parent::offsetGet($key);
    }
}

$arr = new MyArray(1);
var_dump(isset($arr[0]));
$arr[0] = "abc";
var_dump(isset($arr[0]));
var_dump($arr[0]);

?>
--EXPECT--
bool(false)
bool(true)
string(10) "prefix_abc"
