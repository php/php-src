--TEST--
Get property hook by ref and indirect modification
--FILE--
<?php

class Test {
    public $byVal {
        get { return $this->byVal; }
        set { $this->byVal = $value; }
    }
}

$test = new Test;

try {
    $test->byVal = [];
    $test->byVal[] = 42;
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->byVal);

try {
    $test->byVal =& $ref;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Indirect modification of Test::$byVal is not allowed
array(0) {
}
Cannot assign by reference to overloaded object
