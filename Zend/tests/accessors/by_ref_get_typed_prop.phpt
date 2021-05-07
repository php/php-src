--TEST--
By-ref getter with a typed property
--FILE--
<?php

class Test {
    public array $prop = [] { &get; set; }
}

$test = new Test;
$test->prop[] = 42;
var_dump($test->prop);
$ref =& $test->prop;
$ref = [1, 2, 3];
var_dump($test->prop);

try {
    $ref = 42;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
Cannot assign int to reference held by property Test::$prop of type array
