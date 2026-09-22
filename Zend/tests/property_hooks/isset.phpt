--TEST--
isset() and empty() call get property hook
--FILE--
<?php

class Test {
    public $prop1 {
        get { return $this->prop1; }
        set { $this->prop1 = $value; }
    }
}

$test = new Test;

$test->prop1 = true;
var_dump(isset($test->prop1));
var_dump(!empty($test->prop1));
echo "\n",
$test->prop1 = false;
var_dump(isset($test->prop1));
var_dump(!empty($test->prop1));
echo "\n",
$test->prop1 = null;
var_dump(isset($test->prop1));
var_dump(!empty($test->prop1));
echo "\n";

?>
--EXPECT--
bool(true)
bool(true)

bool(true)
bool(false)

bool(false)
bool(false)
