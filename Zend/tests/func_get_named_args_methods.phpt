--TEST--
Testing func_get_named_args() with methods
--FILE--
<?php

class TestClass {
    public function instanceMethod($a, $b = 'default_b', $c = 'default_c') {
        return func_get_named_args();
    }
    
    public static function staticMethod($x, $y = 'default_y', $z = 'default_z') {
        return func_get_named_args();
    }
    
    public function variadicMethod($first, ...$rest) {
        return func_get_named_args();
    }
}

$obj = new TestClass();

var_dump($obj->instanceMethod('A', 'B', 'C'));
var_dump($obj->instanceMethod(c: 'C', a: 'A'));
var_dump($obj->instanceMethod('A', c: 'C'));

var_dump(TestClass::staticMethod('X', 'Y', 'Z'));
var_dump(TestClass::staticMethod(z: 'Z', x: 'X'));

var_dump($obj->variadicMethod('FIRST', 'extra1', 'extra2'));
var_dump($obj->variadicMethod(first: 'FIRST', extra: 'EXTRA'));

?>
--EXPECT--
array(3) {
  ["a"]=>
  string(1) "A"
  ["b"]=>
  string(1) "B"
  ["c"]=>
  string(1) "C"
}
array(3) {
  ["a"]=>
  string(1) "A"
  ["b"]=>
  string(9) "default_b"
  ["c"]=>
  string(1) "C"
}
array(3) {
  ["a"]=>
  string(1) "A"
  ["b"]=>
  string(9) "default_b"
  ["c"]=>
  string(1) "C"
}
array(3) {
  ["x"]=>
  string(1) "X"
  ["y"]=>
  string(1) "Y"
  ["z"]=>
  string(1) "Z"
}
array(3) {
  ["x"]=>
  string(1) "X"
  ["y"]=>
  string(9) "default_y"
  ["z"]=>
  string(1) "Z"
}
array(3) {
  ["first"]=>
  string(5) "FIRST"
  [1]=>
  string(6) "extra1"
  [2]=>
  string(6) "extra2"
}
array(2) {
  ["first"]=>
  string(5) "FIRST"
  ["extra"]=>
  string(5) "EXTRA"
}
