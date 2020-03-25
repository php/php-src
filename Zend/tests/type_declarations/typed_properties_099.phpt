--TEST--
Check that iterating a typed property by reference adds a type source
--FILE--
<?php

class Test {
    public array $ary = [];
}

$test = new Test;
foreach ($test->ary as &$value) {}
var_dump($test);

?>
--EXPECT--
object(Test)#1 (1) {
  ["ary"]=>
  array(0) {
  }
}
