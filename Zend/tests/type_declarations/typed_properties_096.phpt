--TEST--
References to typed properties with undefined classes
--FILE--
<?php

class Test1 {
    public Foobar $prop;
    public int $prop2;
}

$test = new Test1;
$test->prop2 = 123;
$ref =& $test->prop2;
try {
    $test->prop =& $ref;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test);

class Test2 {
    public ?Foobar $prop;
    public ?int $prop2;
}

$test = new Test2;
$test->prop2 = null;
$ref =& $test->prop2;
$test->prop =& $ref;
var_dump($test);

?>
--EXPECT--
Typed property Test1::$prop must be an instance of Foobar, int used
object(Test1)#1 (1) {
  ["prop"]=>
  uninitialized(Foobar)
  ["prop2"]=>
  &int(123)
}
object(Test2)#3 (2) {
  ["prop"]=>
  &NULL
  ["prop2"]=>
  &NULL
}
