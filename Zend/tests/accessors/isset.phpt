--TEST--
isset() and empty() call get accessor
--FILE--
<?php

class Test {
    public $_prop1;
    public $prop1 {
        get { return $this->_prop1; }
        set { $this->_prop1 = $value; }
    }

    public $prop2 {
        get { return $this->prop2; }
        set { $this->prop2 = $value; }
    }

    public array $prop3 { private get; }
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

$test->prop2 = true;
var_dump(isset($test->prop2));
var_dump(!empty($test->prop2));
echo "\n",
$test->prop2 = false;
var_dump(isset($test->prop2));
var_dump(!empty($test->prop2));
echo "\n",
$test->prop2 = null;
var_dump(isset($test->prop2));
var_dump(!empty($test->prop2));
echo "\n";

// Private accessor should simply report as false, not throw
var_dump(isset($test->prop3));
var_dump(!empty($test->prop3));
var_dump(isset($test->prop3[0]));
var_dump(!empty($test->prop3[0]));

?>
--EXPECT--
bool(true)
bool(true)

bool(true)
bool(false)

bool(false)
bool(false)

bool(true)
bool(true)

bool(true)
bool(false)

bool(false)
bool(false)

bool(false)
bool(false)
bool(false)
bool(false)
