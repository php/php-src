--TEST--
Exception in compound assign op should prevent call to overloaded object handlers
--FILE--
<?php

#[AllowDynamicProperties]
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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test);

$test2 = new Test;
try {
    $test2->prop %= 0;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($test2);

?>
--EXPECT--
DivisionByZeroError: Modulo by zero
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    [0]=>
    int(42)
  }
}
DivisionByZeroError: Modulo by zero
object(Test)#3 (1) {
  ["prop"]=>
  int(42)
}
