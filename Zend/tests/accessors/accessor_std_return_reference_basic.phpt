--TEST--
ZE2 Tests that an getter which returns a reference is returning a reference
--FILE--
<?php

class SampleClass {
    public $_dataArray = array(1,2,5,3);
 
    public $dataArray {
        &get { return $this->_dataArray; }
    }
}
 
$o = new SampleClass();
print_r($o->dataArray);
sort($o->dataArray);
print_r($o->_dataArray);
?>
--EXPECTF--
Array
(
    [0] => 1
    [1] => 2
    [2] => 5
    [3] => 3
)
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => 5
)