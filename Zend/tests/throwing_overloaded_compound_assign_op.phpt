--TEST--
Exception in compound assign op should prevent call to overloaded object handlers
--FILE--
<?php

class Test {
    public function __get($k) {
        $this->$k = 42;
        return 0;
    }
}

$test = new ArrayObject;
$test[0] = 42;
try {
    $test[0] %= 0;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test);

$test2 = new Test;
try {
    $test2->prop %= 0;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test2);

?>
--EXPECTF--
Modulo by zero
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    [0]=>
    int(42)
  }
}

Deprecated: Creation of dynamic property Test::$prop is deprecated in %s on line %d
Modulo by zero
object(Test)#3 (1) {
  ["prop"]=>
  int(42)
}
