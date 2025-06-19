--TEST--
Recursion behavior of property hooks
--FILE--
<?php

class Test {
    public int $prop {
        get { return $this->prop * 2; }
        set { $this->prop = $value * 2; }
    }

    // Edge-case where recursion happens via isset().
    public int $prop2 {
        get { return isset($this->prop2); }
        set { }
    }
}

$test = new Test;
$test->prop = 10;
var_dump($test->prop);
var_dump(isset($test->prop));
var_dump(isset($test->prop2));
var_dump($test);

?>
--EXPECTF--
int(40)
bool(true)

Deprecated: Test::$prop2::get(): Returning type bool which is implicitly converted to type int is deprecated in %s on line %d
bool(true)
object(Test)#1 (1) {
  ["prop"]=>
  int(20)
  ["prop2"]=>
  uninitialized(int)
}
