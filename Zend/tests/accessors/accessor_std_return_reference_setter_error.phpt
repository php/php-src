--TEST--
ZE2 Tests that an setter which returns a reference generates a warning
--FILE--
<?php

class SampleClass {
    private $_dataArray = array(1,2,5,3);
 
    public $dataArray {
        &get { return $this->_dataArray; }
        &set { $this->_dataArray = $value; }
    }
}
 
$o = new SampleClass();
print_r($o->dataArray);
sort($o->dataArray);
print_r($o->dataArray);
?>
--EXPECTF--
Warning: Property setter SampleClass::$dataArray indicates a return reference with '&', setters do not return values, ignored. in %s on line %d
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