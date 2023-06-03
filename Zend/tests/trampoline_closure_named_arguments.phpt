--TEST--
Trampoline closure created from magic method accepts named arguments
--FILE--
<?php

class Test {
	public function __call($name, $args) {
        var_dump($name, $args);
    }
	public static function __callStatic($name, $args) {
        var_dump($name, $args);
    }
}

$test = new Test;

echo "-- Non-static cases --\n";
$test->test(1, 2, a: 123);
$test->test(...)(1, 2);
$test->test(...)(1, 2, a: 123, b: $test);
$test->test(...)(a: 123, b: $test);
$test->test(...)();

echo "-- Static cases --\n";
Test::testStatic(1, 2, a: 123);
Test::testStatic(...)(1, 2);
Test::testStatic(...)(1, 2, a: 123, b: $test);
Test::testStatic(...)(a: 123, b: $test);
Test::testStatic(...)();

?>
--EXPECT--
-- Non-static cases --
string(4) "test"
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  ["a"]=>
  int(123)
}
string(4) "test"
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
string(4) "test"
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  ["a"]=>
  int(123)
  ["b"]=>
  object(Test)#1 (0) {
  }
}
string(4) "test"
array(2) {
  ["a"]=>
  int(123)
  ["b"]=>
  object(Test)#1 (0) {
  }
}
string(4) "test"
array(0) {
}
-- Static cases --
string(10) "testStatic"
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  ["a"]=>
  int(123)
}
string(10) "testStatic"
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
string(10) "testStatic"
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  ["a"]=>
  int(123)
  ["b"]=>
  object(Test)#1 (0) {
  }
}
string(10) "testStatic"
array(2) {
  ["a"]=>
  int(123)
  ["b"]=>
  object(Test)#1 (0) {
  }
}
string(10) "testStatic"
array(0) {
}
