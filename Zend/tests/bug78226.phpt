--TEST--
Bug #78226: Unexpected __set behavior with typed properties
--FILE--
<?php

class Test {
    public int $prop1;
    public $prop2;

    public function __set($name, $val) {
        echo "__set($name, $val)\n";
    }
}

$test = new Test;
unset($test->prop2);

$test->prop1 = 1;
$test->prop2 = 2;

var_dump($test);

?>
--EXPECT--
object(Test)#1 (2) {
  ["prop1"]=>
  int(1)
  ["prop2"]=>
  int(2)
}
