--TEST--
Testing func_get_named_args()
--FILE--
<?php

function test($a, $b = 'default_b', $c = 'default_c') {
    return func_get_named_args();
}

var_dump(test('A', 'B', 'C'));
var_dump(test(c: 'C', a: 'A'));
var_dump(test('A', c: 'C'));

function variadic_test($a, $b, ...$rest) {
    return func_get_named_args();
}

var_dump(variadic_test('A', 'B', 'C', 'D'));

function no_args() {
    return func_get_named_args();
}

var_dump(no_args());

try {
    func_get_named_args();
} catch (Error $e) {
    echo "Error: " . $e->getMessage() . "\n";
}

function by_ref($a, &$b) {
    return func_get_named_args();
}

var_dump(by_ref('A', $b));

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
array(4) {
  ["a"]=>
  string(1) "A"
  ["b"]=>
  string(1) "B"
  [2]=>
  string(1) "C"
  [3]=>
  string(1) "D"
}
array(0) {
}
Error: func_get_named_args() cannot be called from the global scope
array(2) {
  ["a"]=>
  string(1) "A"
  ["b"]=>
  NULL
}
