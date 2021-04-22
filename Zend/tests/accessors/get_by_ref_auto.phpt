--TEST--
Get by reference with generated accessors
--FILE--
<?php

class Test {
    public $byVal = [] { get; set; }
    public $byRef = [] { &get; set; }
}

$test = new Test;

$test->byRef[] = 42;
var_dump($test->byRef);

$test->byVal[] = 42;
var_dump($test->byVal);

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
Cannot assign by reference to overloaded object
