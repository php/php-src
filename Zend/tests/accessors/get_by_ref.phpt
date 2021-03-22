--TEST--
Get accessor by ref and indirect modification
--FILE--
<?php

class Test {
    public $_byVal = [];
    public $byVal {
        get { return $this->_byVal; }
    }

    public $_byRef = [];
    public $byRef {
        &get { return $this->_byRef; }
    }

    public int $byRefType {
        &get { $var = "42"; return $var; }
    }
}

$test = new Test;
$test->byRef[] = 42;
var_dump($test->byRef);

$test->byVal[] = 42;
var_dump($test->byVal);

var_dump($test->byRefType);

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
