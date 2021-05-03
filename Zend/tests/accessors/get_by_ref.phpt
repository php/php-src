--TEST--
Get accessor by ref and indirect modification
--FILE--
<?php

class Test {
    public $_byVal = [];
    public $byVal {
        get { return $this->_byVal; }
        set { $this->_byVal = $value; }
    }

    public $_byRef = [];
    public $byRef {
        &get { return $this->_byRef; }
        set { $this->_byRef = $value; }
    }

    public int $byRefType {
        &get { $var = "42"; return $var; }
        set { }
    }
}

$test = new Test;
$test->byRef[] = 42;
var_dump($test->byRef);

$test->byVal[] = 42;
var_dump($test->byVal);

var_dump($test->byRefType);

try {
    $test->byRef =& $ref;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
array(1) {
  [0]=>
  int(42)
}

Notice: Indirect modification of accessor property Test::$byVal has no effect (did you mean to use "&get"?) in %s on line %d
array(0) {
}
int(42)
Cannot assign by reference to overloaded object
