--TEST--
Different kinds of indirect modification with by-val and by-ref getters
--FILE--
<?php


class Test {
    private $_byVal;
    public $byVal {
        get { return $this->_byVal; }
        set {
            echo __METHOD__, "\n";
            $this->_byVal = $value;
        }
    }

    private $_byRef;
    public $byRef {
        &get { return $this->_byRef; }
        set {
            echo __METHOD__, "\n";
            $this->_byRef = $value;
        }
    }
}

$test = new Test;

$test->byVal = 0;
$test->byVal++;
++$test->byVal;
$test->byVal += 1;
var_dump($test->byVal);
$test->byVal = [];
$test->byVal[] = 1;
var_dump($test->byVal);
$ref =& $test->byVal;
$ref = 42;
var_dump($test->byVal);
echo "\n";

$test->byRef = 0;
$test->byRef++;
++$test->byRef;
$test->byRef += 1;
var_dump($test->byRef);
$test->byRef = [];
$test->byRef[] = 1;
var_dump($test->byRef);
$ref =& $test->byRef;
$ref = 42;
var_dump($test->byRef);

?>
--EXPECTF--
Test::$byVal::set
Test::$byVal::set
Test::$byVal::set
Test::$byVal::set
int(3)
Test::$byVal::set

Notice: Indirect modification of accessor property Test::$byVal has no effect (did you mean to use "&get"?) in %s on line %d
array(0) {
}

Notice: Indirect modification of accessor property Test::$byVal has no effect (did you mean to use "&get"?) in %s on line %d
array(0) {
}

Test::$byRef::set
Test::$byRef::set
Test::$byRef::set
Test::$byRef::set
int(3)
Test::$byRef::set
array(1) {
  [0]=>
  int(1)
}
int(42)
